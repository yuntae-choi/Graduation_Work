// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ClientSocket.h"

#pragma comment(lib, "ws2_32.lib")


AMyPlayerController::AMyPlayerController()
{

	myClientSocket = ClientSocket::GetSingleton();
	/*myClientSocket->h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(myClientSocket->_socket), myClientSocket->h_iocp, 0, 0);
	*///int ret = myClientSocket->Connect();
	//if (ret)
	//{
		//UE_LOG(LogClass, Log, TEXT("IOCP Server connect success!"));
		myClientSocket->SetPlayerController(this);
		
	//}


	PrimaryActorTick.bCanEverTick = true;
}


void AMyPlayerController::BeginPlay()
{
	//Super::BeginPlay(); //게임 종료가 안됨

	//auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	//if (!m_Player)
	//	return;
	//auto MyLocation = m_Player->GetActorLocation();
	//auto MyRotation = m_Player->GetActorRotation();

	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	auto MyLocation = m_Player->GetActorLocation();
	auto MyRotation = m_Player->GetActorRotation();
	myClientSocket->fMy_x = MyLocation.X;
	myClientSocket->fMy_y = MyLocation.Y;
	myClientSocket->fMy_z = MyLocation.Z;
	myClientSocket->StartListen();
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	MYLOG(Warning, TEXT("EndPlay!"));
	myClientSocket->LogoutPlayer(iMySessionId);
	myClientSocket->CloseSocket();
	myClientSocket->StopListen();
	//Super::EndPlay(EndPlayReason);
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//새플레이어
	if (!iNewPlayers.empty())
		UpdateNewPlayer();
	//새 눈덩이
	if (!iNewBalls.empty())
		UpdateNewBall();
	// 월드 동기화
	UpdateWorldInfo();
	//UpdateRotation();

}

//타플레이어 정보 수정
bool AMyPlayerController::UpdateWorldInfo()
{
	UWorld* const world = GetWorld();
	if (world == nullptr)
		return false;

	if (CharactersInfo == nullptr)
		return false;

	// 플레이어 업데이트
	UpdatePlayerInfo(CharactersInfo->players[iMySessionId]);

	// 다른 플레이어 업데이트
	TArray<AActor*> SpawnedCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyCharacter::StaticClass(), SpawnedCharacters);

	if (nPlayers == -1)
	{
		for (auto& player : CharactersInfo->players)
		{
			if (player.first == iMySessionId || !player.second.IsAlive)
				continue;

			FVector SpawnLocation_;
			SpawnLocation_.X = player.second.X;
			SpawnLocation_.Y = player.second.Y;
			SpawnLocation_.Z = player.second.Z;

			FRotator SpawnRotation;
			SpawnRotation.Yaw = player.second.Yaw;
			SpawnRotation.Pitch = player.second.Pitch;
			SpawnRotation.Roll = player.second.Roll;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.Name = FName(*FString(to_string(player.second.SessionId).c_str()));

			AMyCharacter* SpawnCharacter = world->SpawnActor<AMyCharacter>(WhoToSpawn, SpawnLocation_, SpawnRotation, SpawnParams);
			SpawnCharacter->SpawnDefaultController();

			SpawnCharacter->iSessionID = player.second.SessionId;
			SpawnCharacter->SessionId = player.second.SessionId;

			//SpawnCharacter->HealthValue = player.second.HealthValue;
			SpawnCharacter->IsAlive = player.second.IsAlive;
			//SpawnCharacter->IsAttacking = player.second.IsAttacking;
		}

		nPlayers = CharactersInfo->players.size();
	}
	else
	{
		for (auto& Character_ : SpawnedCharacters)
		{
			AMyCharacter* OtherPlayer = Cast<AMyCharacter>(Character_);

			if (!OtherPlayer || OtherPlayer->iSessionID == -1 || OtherPlayer->iSessionID == iMySessionId)
			{
				continue;
			}

			//타플레이어
			cCharacter* info = &CharactersInfo->players[OtherPlayer->iSessionID];

			if (info->IsAlive)
			{
				//if (OtherPlayer->HealthValue != info->HealthValue)
				//{
				//	UE_LOG(LogClass, Log, TEXT("other player damaged."));
				//	// 피격 파티클 소환
				//	FTransform transform(OtherPlayer->GetActorLocation());
				//	UGameplayStatics::SpawnEmitterAtLocation(
				//		world, HitEmiiter, transform, true
				//	);
				//	// 피격 애니메이션 플레이
				//	OtherPlayer->PlayDamagedAnimation();
				//	OtherPlayer->HealthValue = info->HealthValue;
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
				CharacterRotation.Pitch = info->Pitch;
				CharacterRotation.Roll = info->Roll;

				FVector CharacterVelocity;
				CharacterVelocity.X = info->VX;
				CharacterVelocity.Y = info->VY;
				CharacterVelocity.Z = info->VZ;

				OtherPlayer->AddMovementInput(CharacterVelocity);
				OtherPlayer->SetActorRotation(CharacterRotation);
				OtherPlayer->SetActorLocation(CharacterLocation);
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

	}


	return true;
}



//플레이어 정보 업데이트
void AMyPlayerController::StartPlayerInfo(const cCharacter& info)
{
	auto Player_ = Cast<AMyCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!Player_)
		return;


	UWorld* const world = GetWorld();
	if (!world)
		return;

	if (bSetPlayer) {
		iMySessionId = info.SessionId;
		FVector _CharacterLocation;
		_CharacterLocation.X = info.X;
		_CharacterLocation.Y = info.Y;
		_CharacterLocation.Z = info.Z;
		Player_->SetActorLocation(_CharacterLocation);
		bSetPlayer = false;
	}

	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	auto MyLocation = m_Player->GetActorLocation();
	auto MyRotation = m_Player->GetActorRotation();

	myClientSocket->fMy_x = MyLocation.X;
	myClientSocket->fMy_y = MyLocation.Y;
	myClientSocket->fMy_z = MyLocation.Z;
	MYLOG(Warning, TEXT("i'm player init spawn : (%f, %f, %f)"), MyLocation.X, MyLocation.Y, MyLocation.Z);


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
		//// 캐릭터 속성 업데이트
		//if (Player->HealthValue != info.HealthValue)
		//{
		//	UE_LOG(LogClass, Log, TEXT("Player damaged"));
		//	// 피격 파티클 스폰
		//	FTransform transform(Player->GetActorLocation());
		//	UGameplayStatics::SpawnEmitterAtLocation(
		//		world, HitEmiiter, transform, true
		//	);
		//	// 피격 애니메이션 스폰
		//	Player->PlayDamagedAnimation();
		//	Player->HealthValue = info.HealthValue;
		//}
	}
}

//플레이어 정보 업데이트
void AMyPlayerController::UpdatePlayerInfo(const cCharacter& info)
{
	auto Player_ = Cast<AMyCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!Player_)
		return;
	
	
	UWorld* const world = GetWorld();
	if (!world)
		return;

	if (bSetPlayer) {
		FVector _CharacterLocation;
		_CharacterLocation.X = info.X;
		_CharacterLocation.Y = info.Y;
		_CharacterLocation.Z = info.Z;
		Player_->SetActorLocation(_CharacterLocation);
		bSetPlayer = false;
	}

	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	auto MyLocation = m_Player->GetActorLocation();
	auto MyRotation = m_Player->GetActorRotation();
	
	myClientSocket->fMy_x = MyLocation.X;
	myClientSocket->fMy_y = MyLocation.Y;
	myClientSocket->fMy_z = MyLocation.Z;
	//MYLOG(Warning, TEXT("i'm player init spawn : (%f, %f, %f)"), MyLocation.X, MyLocation.Y, MyLocation.Z);


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
		//// 캐릭터 속성 업데이트
		//if (Player->HealthValue != info.HealthValue)
		//{
		//	UE_LOG(LogClass, Log, TEXT("Player damaged"));
		//	// 피격 파티클 스폰
		//	FTransform transform(Player->GetActorLocation());
		//	UGameplayStatics::SpawnEmitterAtLocation(
		//		world, HitEmiiter, transform, true
		//	);
		//	// 피격 애니메이션 스폰
		//	Player->PlayDamagedAnimation();
		//	Player->HealthValue = info.HealthValue;
		//}
	}
}

void AMyPlayerController::UpdatePlayerInfo(int input)
{
	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	iMySessionId = m_Player->iSessionID;
	auto MyLocation = m_Player->GetActorLocation();
	auto MyRotation = m_Player->GetActorRotation();
	auto MyVelocity = m_Player->GetVelocity();
	FVector MyCameraLocation;
	FRotator MyCameraRotation;
	m_Player->GetActorEyesViewPoint(MyCameraLocation, MyCameraRotation);
	if (input == COMMAND_MOVE)
		myClientSocket->ReadyToSend_MovePacket(iMySessionId, MyLocation, MyRotation, MyVelocity);
	else if (input == COMMAND_ATTACK) 
		myClientSocket->ReadyToSend_Throw_Packet(iMySessionId, MyCameraLocation, MyCameraRotation.Vector());
	else if (input == COMMAND_DAMAGE)
		myClientSocket->ReadyToSend_DamgePacket();

}

void AMyPlayerController::UpdateFarming(int item_no)
{
		myClientSocket->ReadyToSend_ItemPacket(item_no);
}

void AMyPlayerController::UpdatePlayerS_id(int id)
{
	iMySessionId = id;
	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	m_Player->iSessionID = id;
	m_Player->SessionId = id;
	m_Player->SetActorLocationAndRotation(FVector(id * 100.0f, id * 100.0f, m_Player->GetActorLocation().Z), FRotator(0.0f, -90.0f, 0.0f));
}

void AMyPlayerController::RecvNewPlayer(const cCharacter& info)
{
	//MYLOG(Warning, TEXT("recv ok player%d : %f, %f, %f"), sessionID, x, y, z);

	UWorld* World = GetWorld();
	iNewPlayers.push(info.SessionId);
}

void AMyPlayerController::RecvNewBall(int s_id)
{
	//MYLOG(Warning, TEXT("recv ok player%d : %f, %f, %f"), sessionID, x, y, z);

	UWorld* World = GetWorld();
	iNewBalls.push(s_id);
}

void AMyPlayerController::UpdateNewPlayer()
{
	UWorld* const World = GetWorld();

	// 새로운 플레이어가 자기 자신이면 무시
	int new_s_id = iNewPlayers.front();
	if (new_s_id== iMySessionId)
	{
		iNewPlayers.pop();

		return;
	}

	// 새로운 플레이어를 필드에 스폰
	FVector SpawnLocation_;
	SpawnLocation_.X = CharactersInfo->players[new_s_id].X;
	SpawnLocation_.Y = CharactersInfo->players[new_s_id].Y;
	SpawnLocation_.Z = CharactersInfo->players[new_s_id].Z;

	FRotator SpawnRotation;
	SpawnRotation.Yaw = CharactersInfo->players[new_s_id].Yaw;
	SpawnRotation.Pitch = CharactersInfo->players[new_s_id].Pitch;
	SpawnRotation.Roll = CharactersInfo->players[new_s_id].Roll;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	//SpawnParams.Name = FName(*FString(to_string(iOtherSessionId).c_str()));

	WhoToSpawn = AMyCharacter::StaticClass();
	AMyCharacter* SpawnCharacter = World->SpawnActor<AMyCharacter>(WhoToSpawn, SpawnLocation_, SpawnRotation, SpawnParams);

	if (nullptr == SpawnCharacter)
	{
		MYLOG(Warning, TEXT("spawn fail"));
		return;
	}
	//MYLOG(Warning, TEXT("spawn ok player%d : %f, %f, %f"), iOtherSessionId, fOther_x, fOther_y, fOther_z);

	SpawnCharacter->SpawnDefaultController();
	SpawnCharacter->iSessionID = new_s_id;
	iNewPlayers.pop();

}

void AMyPlayerController::UpdateNewBall()
{
	UWorld* const World = GetWorld();
	// 새로운 플레이어가 자기 자신이면 무시
	int new_s_id = iNewBalls.front();
	FVector C_Location_;
	C_Location_.X = CharactersInfo->players[new_s_id].fCx;
	C_Location_.Y = CharactersInfo->players[new_s_id].fCy;
	C_Location_.Z = CharactersInfo->players[new_s_id].fCz;
	FVector CD_Location_;
	CD_Location_.X = CharactersInfo->players[new_s_id].fCDx;
	CD_Location_.Y = CharactersInfo->players[new_s_id].fCDy;
	CD_Location_.Z = CharactersInfo->players[new_s_id].fCDz;
	CharactersInfo->players[new_s_id].FMyLocation = C_Location_;
	CharactersInfo->players[new_s_id].FMyDirection = CD_Location_;
	
	if (new_s_id == iMySessionId)
	{
		auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		
		m_Player->SnowAttack();
		iNewBalls.pop();
		return;
	}

	TArray<AActor*> SpawnedCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyCharacter::StaticClass(), SpawnedCharacters);

	for (auto& Character_ : SpawnedCharacters)
	{
		AMyCharacter* OtherPlayer = Cast<AMyCharacter>(Character_);

		if (!OtherPlayer || OtherPlayer->iSessionID == -1 || new_s_id == iMySessionId )
		{
			continue;
		}
        
		if (new_s_id == OtherPlayer->iSessionID) {
			cCharacter* info = &CharactersInfo->players[new_s_id];
			if (info->IsAlive)
			{				
				OtherPlayer->SnowAttack();
				iNewBalls.pop();
				return;
			}
		}
	}
	iNewPlayers.pop();

}

void AMyPlayerController::Throw_Snow(FVector MyLocation, FVector MyDirection)
{
	myClientSocket->ReadyToSend_Throw_Packet(iMySessionId, MyLocation, MyDirection);

};

//void AMyPlayerController::UpdateRotation()
//{
//	float pitch, yaw, roll;
//	UKismetMathLibrary::BreakRotator(GetControlRotation(), roll, pitch, yaw);
//	pitch = UKismetMathLibrary::ClampAngle(pitch, -15.0f, 30.0f);
//	FRotator newRotator = UKismetMathLibrary::MakeRotator(roll, pitch, yaw);
//	SetControlRotation(newRotator);
//}
