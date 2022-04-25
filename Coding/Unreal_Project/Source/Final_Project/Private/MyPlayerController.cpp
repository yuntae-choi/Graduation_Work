// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "MyGameModeBase.h"
#include "ClientSocket.h"
#include "MyAnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

#pragma comment(lib, "ws2_32.lib")


AMyPlayerController::AMyPlayerController()
{
	mySocket = ClientSocket::GetSingleton();
	mySocket->SetPlayerController(this);

	bNewPlayerEntered = false;
	bInitPlayerSetting = false;

	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> READY_UI(TEXT("/Game/Blueprints/ReadyUI.ReadyUI_C"));
	if (READY_UI.Succeeded() && (READY_UI.Class != nullptr))
	{
		readyUIClass = READY_UI.Class;
	}

	bIsReady = false;
}


void AMyPlayerController::BeginPlay()
{
	MYLOG(Warning, TEXT("BeginPlay!"));
	mySocket->StartListen();

	// 실행시 클릭없이 바로 조작
	//FInputModeGameOnly InputMode;
	//SetInputMode(InputMode);

	LoadReadyUI();	// readyUI 띄우고 게임에 대한 입력 x, UI에 대한 입력만 받음
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//MYLOG(Warning, TEXT("EndPlay!"));
	//mySocket->Send_LogoutPacket(iSessionId);
	//mySocket->CloseSocket();
	//mySocket->StopListen();
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//플레이어 초기설정
	if (bInitPlayerSetting)
		InitPlayerSetting();

	//새 플레이어 스폰
	if (bNewPlayerEntered)
		UpdateNewPlayer();
	
	// 월드 동기화
	UpdateWorldInfo();

	//UpdateRotation();
}

void AMyPlayerController::SetInitInfo(const cCharacter& me)
{
	initInfo = me;
	bInitPlayerSetting = true;
}

void AMyPlayerController::SetNewCharacterInfo(shared_ptr<cCharacter> NewPlayer_)
{
	if (NewPlayer_ != nullptr)
	{
		bNewPlayerEntered = true;
		newPlayers.push(NewPlayer_);
	}
}

void AMyPlayerController::SetNewBall(const int s_id)
{
	UWorld* World = GetWorld();
	newBalls.push(s_id);
}

void AMyPlayerController::InitPlayerSetting()
{
	auto player_ = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!player_) return;
	player_->SetActorLocation(FVector(initInfo.X, initInfo.Y, initInfo.Z));
	player_->iSessionId = initInfo.SessionId;

	//컨트롤러는 초기설정이 불가능함
	SetControlRotation(FRotator(0.0f, initInfo.Yaw, 0.0f));

	player_->SetCharacterMaterial(iSessionId);

	bInitPlayerSetting = false;
}

bool AMyPlayerController::UpdateWorldInfo()
{
	UWorld* const world = GetWorld();
	if (world == nullptr)					return false;
	if (charactersInfo == nullptr)	return false;

	// 플레이어자신 체력, 사망업데이트
	UpdatePlayerInfo(charactersInfo->players[iSessionId]);

	if (charactersInfo->players.size() == 1)
	{
		//MYLOG(Warning, TEXT("Only one player"));
		return false;
	}

	// 스폰캐릭터들배열 하나 생성하고 월드에 있는 캐릭터들을 배열에 넣어주기
	TArray<AActor*> SpawnedCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyCharacter::StaticClass(), SpawnedCharacters);

	for (auto& Character_ : SpawnedCharacters)
	{
		//자신포함 모든플레이어
		AMyCharacter* player_ = Cast<AMyCharacter>(Character_);

		//새 눈덩이
		if (!newBalls.empty() && newBalls.front() == player_->iSessionId)
		{
			player_->SnowAttack();
			newBalls.pop();
		}

		if (!player_ || player_->iSessionId == -1 || player_->iSessionId == iSessionId)
		{
			continue;
		}

		//타플레이어
		cCharacter* info = &charactersInfo->players[player_->iSessionId];

		if (info->IsAlive)
		{
			//if (player_->iCurrentHP != info->HealthValue)
			//{
			//	MYLOG(Warning, TEXT("other player damaged."));
			//	//// 피격 파티클 소환
			//	//FTransform transform(OtherPlayer->GetActorLocation());
			//	//UGameplayStatics::SpawnEmitterAtLocation(
			//	//	world, HitEmiiter, transform, true
			//	//);
			//	//// 피격 애니메이션 플레이
			//	//player_->PlayDamagedAnimation();
			//	//player_->iCurrentHP = info->HealthValue;
			//}

			//// 공격중일때 타격 애니메이션 플레이
			//if (info->IsAttacking)
			//{
			//	UE_LOG(LogClass, Log, TEXT("other player hit."));
			//	OtherPlayer->PlayHitAnimation();
			//}

			FVector CharacterLocation;
			CharacterLocation.X = info->X;
			CharacterLocation.Y = info->Y;
			CharacterLocation.Z = info->Z;

			FRotator CharacterRotation;
			CharacterRotation.Yaw = info->Yaw;
			CharacterRotation.Pitch = 0.0f;
			CharacterRotation.Roll = 0.0f;

			FVector CharacterVelocity;
			CharacterVelocity.X = info->VX;
			CharacterVelocity.Y = info->VY;
			CharacterVelocity.Z = info->VZ;

			player_->AddMovementInput(CharacterVelocity);
			player_->SetActorRotation(CharacterRotation);
			player_->SetActorLocation(CharacterLocation);
			player_->GetAnim()->SetDirection(info->direction);

			//눈사람 변화
			if (!player_->IsSnowman())
			{
				if (info->My_State == ST_SNOWMAN)
				{
					player_->ChangeSnowman();
				}
			}
			else {
				if (info->My_State != ST_SNOWMAN)
				{
					player_->ChangeAnimal();
				}
			}
		}
		else
		{
			/*	UE_LOG(LogClass, Log, TEXT("other player dead."));
				FTransform transform(Character->GetActorLocation());
				UGameplayStatics::SpawnEmitterAtLocation(
					world, DestroyEmiiter, transform, true
				);
				Character->Destroy();*/
		}
	}
	return true;
}

void AMyPlayerController::UpdateNewPlayer()
{
	UWorld* const World = GetWorld();

	int size_ = newPlayers.size();

	for (int i = 0; i < size_; ++i)
	{
		// 새로운 플레이어가 자기 자신이면 무시
		if (newPlayers.front()->SessionId == iSessionId)
		{
			newPlayers.front() = nullptr;
			newPlayers.pop();
			continue;
		}

		// 새로운 플레이어를 필드에 스폰
		FVector SpawnLocation_;
		SpawnLocation_.X = newPlayers.front()->X;
		SpawnLocation_.Y = newPlayers.front()->Y;
		SpawnLocation_.Z = newPlayers.front()->Z;

		FRotator SpawnRotation;
		SpawnRotation.Yaw = newPlayers.front()->Yaw;
		SpawnRotation.Pitch = 0.0f;
		SpawnRotation.Roll = 0.0f;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.Name = FName(*FString(to_string(newPlayers.front()->SessionId).c_str()));

		WhoToSpawn = AMyCharacter::StaticClass();
		AMyCharacter* SpawnCharacter = World->SpawnActor<AMyCharacter>(WhoToSpawn, SpawnLocation_, SpawnRotation, SpawnParams);
		SpawnCharacter->SpawnDefaultController();
		SpawnCharacter->iSessionId = newPlayers.front()->SessionId;
		SpawnCharacter->SetCharacterMaterial(SpawnCharacter->iSessionId);

		// 필드의 플레이어 정보에 추가
		if (charactersInfo != nullptr)
		{
			cCharacter info;
			info.SessionId = newPlayers.front()->SessionId;
			info.X = newPlayers.front()->X;
			info.Y = newPlayers.front()->Y;
			info.Z = newPlayers.front()->Z;

			info.Yaw = newPlayers.front()->Yaw;

			charactersInfo->players[newPlayers.front()->SessionId] = info;
		}

		MYLOG(Warning, TEXT("other player(id : %d) spawned."), newPlayers.front()->SessionId);

		newPlayers.front() = nullptr;
		newPlayers.pop();
	}
	bNewPlayerEntered = false;
}

void AMyPlayerController::SendPlayerInfo(int input)
{
	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	auto MyLocation = m_Player->GetActorLocation();
	auto MyRotation = m_Player->GetActorRotation();
	auto MyVelocity = m_Player->GetVelocity();
	FVector MyCameraLocation;
	FRotator MyCameraRotation;
	m_Player->GetActorEyesViewPoint(MyCameraLocation, MyCameraRotation);
	float dir = m_Player->GetAnim()->GetDirection();

	if (input == COMMAND_MOVE)
		mySocket->Send_MovePacket(iSessionId, MyLocation, MyRotation, MyVelocity, dir);
	else if (input == COMMAND_ATTACK) 
		mySocket->Send_Throw_Packet(iSessionId, MyCameraLocation, MyCameraRotation.Vector());
	else if (input == COMMAND_DAMAGE)
		mySocket->Send_DamagePacket();
}

//플레이어 정보 업데이트
void AMyPlayerController::UpdatePlayerInfo(const cCharacter& info)
{
	UWorld* const world = GetWorld();
	if (!world)
		return;

	auto player_ = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!player_)
		return;

	if (!info.IsAlive)
	{
		/*UE_LOG(LogClass, Log, TEXT("Player Die"));
		FTransform transform(Player->GetActorLocation());
		UGameplayStatics::SpawnEmitterAtLocation(
			world, DestroyEmiiter, transform, true
		);
		Player->Destroy();

		CurrentWidget->RemoveFromParent();
		GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
		if (GameOverWidget != nullptr)
		{
			GameOverWidget->AddToViewport();
		}*/
	}
	else
	{
		// 캐릭터 속성 업데이트
		if (player_->iCurrentHP != info.HealthValue)
		{
			//MYLOG(Warning, TEXT("Player damaged hp: %d"), info.HealthValue);
			player_->iCurrentHP = info.HealthValue;


			//// 피격 파티클 스폰
			//FTransform transform(player_->GetActorLocation());
			//UGameplayStatics::SpawnEmitterAtLocation(
			//	world, HitEmiiter, transform, true
			//);
			// 피격 애니메이션 스폰
			//player_->PlayDamagedAnimation();
			//player_->HealthValue = info.HealthValue;
		}
	}
}


//void AMyPlayerController::UpdateFarming(int item_no)
//{
//		mySocket->ReadyToSend_ItemPacket(item_no);
//}
//
//void AMyPlayerController::UpdatePlayerS_id(int id)
//{
//	iMySessionId = id;
//	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
//	if (!m_Player)
//		return;
//	m_Player->iSessionID = id;
//	m_Player->SessionId = id;
//	m_Player->SetActorLocationAndRotation(FVector(id * 100.0f, id * 100.0f, m_Player->GetActorLocation().Z), FRotator(0.0f, -90.0f, 0.0f));
//}
//
//void AMyPlayerController::RecvNewPlayer(const cCharacter& info)
//{
//	//MYLOG(Warning, TEXT("recv ok player%d : %f, %f, %f"), sessionID, x, y, z);
//
//	UWorld* World = GetWorld();
//	iNewPlayers.push(info.SessionId);
//}

void AMyPlayerController::LoadReadyUI()
{
	if (readyUIClass)
	{
		readyUI = CreateWidget<UUserWidget>(GetWorld(), readyUIClass);
		if (readyUI)
		{
			readyUI->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
			bShowMouseCursor = true;
		}
	}
}

void AMyPlayerController::PlayerReady()
{
	// 서버에 레디했다고 전송
	UE_LOG(LogTemp, Warning, TEXT("PlayerReady"));
	bIsReady = true;
}

void AMyPlayerController::PlayerUnready()
{
	// 서버에 언레디했다고 전송
	UE_LOG(LogTemp, Warning, TEXT("PlayerUnready"));
	bIsReady = false;
}

void AMyPlayerController::StartGame()
{
	readyUI->RemoveFromParent();	// ReadyUI 제거

	// 실행시 클릭없이 바로 조작
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	// 게임 시작되면 실행시킬 코드들 작성
}