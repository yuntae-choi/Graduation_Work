// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "MyGameModeBase.h"
#include "ClientSocket.h"
#include "MyAnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Debug.h"
#include "Snowdrift.h"
#include "Icedrift.h"
#include "Itembox.h"
#include "SupplyBox.h"
#include "Tornado.h"

#pragma comment(lib, "ws2_32.lib")

ClientSocket* g_socket = nullptr;

void CALLBACK recv_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
	//MYLOG(Warning, TEXT("recv_callback"));
	if (num_byte == 0) {
		g_socket->CloseSocket();
		g_socket = nullptr;
		MYLOG(Warning, TEXT("recv_error"));

		return;
	}
	unsigned char* packet_start = g_socket->_recv_over._net_buf;
	int packet_size = packet_start[0];
	int remain_data = num_byte + g_socket->_prev_size;
	g_socket->process_data(g_socket->_recv_over._net_buf, remain_data);
	recv_flag = 0;
	ZeroMemory(&g_socket->_recv_over._wsa_over, sizeof(g_socket->_recv_over._wsa_over));
	g_socket->_recv_over._wsa_buf.buf = reinterpret_cast<char*>(g_socket->_recv_over._net_buf + g_socket->_prev_size);
	g_socket->_recv_over._wsa_buf.len = sizeof(g_socket->_recv_over._net_buf) - g_socket->_prev_size;
	WSARecv(g_socket->_socket, &g_socket->_recv_over._wsa_buf, 1, 0, &recv_flag, &g_socket->_recv_over._wsa_over, recv_callback);
	SleepEx(0, true);
}

AMyPlayerController::AMyPlayerController()
{
	//bNewPlayerEntered = false;
	bInitPlayerSetting = false;
	bSetStart.store(false);
	ivictoryPlayer.store(-1);
	bInGame = false;
	PrimaryActorTick.bCanEverTick = true;
	iBearCnt = 0;
	iSnowmanCnt = 0;
	bTornado = false;

	static ConstructorHelpers::FClassFinder<UUserWidget> READY_UI(TEXT("/Game/Blueprints/ReadyUI.ReadyUI_C"));
	if (READY_UI.Succeeded() && (READY_UI.Class != nullptr))
	{
		readyUIClass = READY_UI.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> CHARACTER_UI(TEXT("/Game/Blueprints/CharacterUI.CharacterUI_C"));
	if (CHARACTER_UI.Succeeded() && (CHARACTER_UI.Class != nullptr))
	{
		characterUIClass = CHARACTER_UI.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> GAMERESULT_UI(TEXT("/Game/Blueprints/GameOverUI.GameOverUI_C"));
	if (GAMERESULT_UI.Succeeded() && (GAMERESULT_UI.Class != nullptr))
	{
		gameResultUIClass = GAMERESULT_UI.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> LOGIN_UI(TEXT("/Game/Blueprints/LoginUI.LoginUI_C"));
	if (LOGIN_UI.Succeeded() && (LOGIN_UI.Class != nullptr))
	{
		loginUIClass = LOGIN_UI.Class;
	}

	if (!supplyboxClass)
	{
		static ConstructorHelpers::FClassFinder<AActor> SupplyBox_Class(TEXT("/Game/Blueprints/SupplyBox_BP.SupplyBox_BP_C"));
		if (SupplyBox_Class.Succeeded())
		{
			supplyboxClass = SupplyBox_Class.Class;
		}
	}

	bIsReady = false;
	loginInfoText = TEXT("LOGIN");
}

void AMyPlayerController::OnPossess(APawn* pawn_)
{
	Super::OnPossess(pawn_);

	//mySocket->StartListen();

	localPlayerCharacter = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	LoadLoginUI();	// loginUI 띄우고 게임에 대한 입력 x, UI에 대한 입력만 받음
	//LoadReadyUI();	// readyUI 띄우고 게임에 대한 입력 x, UI에 대한 입력만 받음

	FPlatformProcess::Sleep(0);
}

void AMyPlayerController::BeginPlay()
{
	MYLOG(Warning, TEXT("BeginPlay!"));

	// 실행시 클릭없이 바로 조작
	//SetSocket();
	//SleepEx(0, true);
	SetSocket();
	//mySocket->StartListen();
	SleepEx(0, true);
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	MYLOG(Warning, TEXT("EndPlay!"));
	//mySocket->Send_LogoutPacket(iSessionId);
	////SleepEx(0, true);
	//mySocket->CloseSocket();
	//g_socket = nullptr;

	// 델리게이트 해제
	FuncUpdateHP.Clear();
	FuncUpdateCurrentSnowballCount.Clear();
	FuncUpdateCurrentIceballCount.Clear();
	FuncUpdateCurrentMatchCount.Clear();
	FuncUpdateMaxSnowIceballAndMatchCount.Clear();
	FuncUpdateHasUmbrella.Clear();
	FuncUpdateHasBag.Clear();
	FuncUpdateHasShotgun.Clear();
	FuncUpdateIsFarmingSnowdrift.Clear();
	FuncUpdateSnowdriftFarmDuration.Clear();
	FuncUpdateSelectedItem.Clear();
	FuncUpdateSelectedProjectile.Clear();
	FuncUpdateSelectedWeapon.Clear();
	FuncUpdateKillLog.Clear();
	FuncUpdateGameResult.Clear();
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	////확인용
	//TArray<AActor*> Itemboxes;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItembox::StaticClass(), Itemboxes);

	//for (auto ib : Itemboxes)
	//{
	//	AItembox* itembox = Cast<AItembox>(ib);

	//	MYLOG(Warning, TEXT("id : %d"), itembox->GetId());
	//}

	// 월드 동기화
	UpdateWorldInfo();

	SendPlayerInfo(COMMAND_MOVE);

	SleepEx(0, true);

	//FixRotation();	// 캐릭터 피칭(상하)제한
}

void AMyPlayerController::SetSocket()
{
	mySocket = new ClientSocket();         // 에디터용
	//mySocket = ClientSocket::GetSingleton(); // 패키징 용

	mySocket->SetPlayerController(this);
	g_socket = mySocket;
	mySocket->Connect();

	DWORD recv_flag = 0;
	ZeroMemory(&g_socket->_recv_over._wsa_over, sizeof(g_socket->_recv_over._wsa_over));
	g_socket->_recv_over._wsa_buf.buf = reinterpret_cast<char*>(g_socket->_recv_over._net_buf + g_socket->_prev_size);
	g_socket->_recv_over._wsa_buf.len = sizeof(g_socket->_recv_over._net_buf) - g_socket->_prev_size;
	WSARecv(g_socket->_socket, &g_socket->_recv_over._wsa_buf, 1, 0, &recv_flag, &g_socket->_recv_over._wsa_over, recv_callback);
	SleepEx(0, true);

}

void AMyPlayerController::SetInitInfo(const cCharacter& me)
{
	initInfo = me;
	bInitPlayerSetting = true;
	InitPlayerSetting();

}

void AMyPlayerController::SetNewCharacterInfo(shared_ptr<cCharacter> NewPlayer_)
{
	if (NewPlayer_ != nullptr)
	{
		newplayer = NewPlayer_;
		UpdateNewPlayer();
	}
}

void AMyPlayerController::SetNewTornadoInfo(shared_ptr<cCharacter> newTornado)
{
	if (newTornado != nullptr)
	{
		MYLOG(Warning, TEXT("SetNewTornadoInfo"));
		//newplayer = newTornado;
		newtornado = newTornado;
		UpdateNewTornado();
	}
}


void AMyPlayerController::SetAttack(const int s_id, int at_type)
{
	UWorld* World = GetWorld();
	switch (at_type)
	{
	case ATTACK_SNOWBALL: {
		charactersInfo->players[s_id].bStartSnowBall = true;
		break;
	}
	case ATTACK_ICEBALL: {
		charactersInfo->players[s_id].bStartIceBall = true;
		break;
	}
	case ATTACK_SHOTGUN: {
		charactersInfo->players[s_id].bStartShotGun = true;
		break;
	}
	case END_SNOWBALL: {
		charactersInfo->players[s_id].bEndSnowBall = true;
		break;
	}
	case END_ICEBALL: {
		charactersInfo->players[s_id].bEndIceBall = true;
		break;
	}
	case END_SHOTGUN: {
		charactersInfo->players[s_id].bEndShotGun = true;
		break;
	}
	case CANCEL_SNOWBALL: {
		charactersInfo->players[s_id].bCancelSnowBall = true;
		break;
	}
	case CANCEL_ICEBALL: {
		charactersInfo->players[s_id].bCancelIceBall = true;
		break;
	}
	default:
		break;
	}
}

void AMyPlayerController::SetItem(const int s_id, int item_type, bool end)
{
	UWorld* World = GetWorld();
	switch (item_type)
	{
	case ITEM_UMB: {
		if (!end) {
			charactersInfo->players[s_id].bStartUmb = true;
		}
		else {
			charactersInfo->players[s_id].bEndUmb = true;
		}
		break;
	}
	case ITEM_JET: {
		charactersInfo->players[s_id].bSetJetSki = true;
		break;
	}
	default:
		break;
	}
}

void AMyPlayerController::SetDestroyPlayer(const int del_sid)
{
	UWorld* World = GetWorld();

	TArray<AActor*> delCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyCharacter::StaticClass(), delCharacters);

	for (auto sd : delCharacters)
	{
		AMyCharacter* Del_Player = Cast<AMyCharacter>(sd);

		if (Del_Player->iSessionId == del_sid)
		{
			charactersInfo->players.erase(Del_Player->iSessionId);
			Del_Player->Destroy();
			Del_Player = nullptr;

		}
	}

}

void AMyPlayerController::SetDestroySnowdritt(const int obj_id)
{
	UWorld* World = GetWorld();
	TArray<AActor*> Snowdrifts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASnowdrift::StaticClass(), Snowdrifts);

	if (obj_id == -1) return;
	for (auto sd : Snowdrifts)
	{
		ASnowdrift* snowdrift = Cast<ASnowdrift>(sd);

		if (snowdrift->GetId() == obj_id)
		{
			snowdrift->Destroy();
			snowdrift = nullptr;
		}
	}

}

void AMyPlayerController::SetDestroyIcedritt(const int obj_id)
{
	UWorld* World = GetWorld();
	TArray<AActor*> Icedrifts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIcedrift::StaticClass(), Icedrifts);

	if (obj_id == -1) return;
	for (auto sd : Icedrifts)
	{
		AIcedrift* icedrift = Cast<AIcedrift>(sd);

		if (icedrift->GetId() == obj_id)
		{
			icedrift->Destroy();
			icedrift = nullptr;
		}
	}

}

void AMyPlayerController::SetOpenItembox(const int obj_id)
{
	UWorld* World = GetWorld();
	TArray<AActor*> ItemBox;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItembox::StaticClass(), ItemBox);

	for (auto sd : ItemBox)
	{
		AItembox* itembox = Cast<AItembox>(sd);
		//if (!itembox) continue;
		if (itembox->GetId() == obj_id)
		{
			itembox->SetItemboxState(ItemboxState::Opening);
		}
	}


}

void AMyPlayerController::SetGameEnd(const int target_id)
{
	UWorld* World = GetWorld();
	LoadGameResultUI(target_id);
}

void AMyPlayerController::SetDestroyitembox(const int obj_id)
{
	UWorld* World = GetWorld();


	TArray<AActor*> ItemBox;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItembox::StaticClass(), ItemBox);

	for (auto sd : ItemBox)
	{
		AItembox* itembox = Cast<AItembox>(sd);

		if (itembox->GetId() == obj_id)
		{
			itembox->DeleteItem();
		}
	}

}

void AMyPlayerController::SetDestroySpBox(const int obj_id)
{
	UWorld* World = GetWorld();


	TArray<AActor*> SpBox;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASupplyBox::StaticClass(), SpBox);

	for (auto sd : SpBox)
	{
		ASupplyBox* spBox = Cast<ASupplyBox>(sd);

		if (spBox->GetId() == obj_id)
		{
			spBox->Destroy();
		}
	}

}

void AMyPlayerController::GetItem(int sId, int itemType)
{

	auto player_ = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!player_)
		return;
	switch (itemType)
	{

	case ITEM_BAG:
	{
		break;
	}
	case ITEM_UMB:
	{

		break;
	}
	case ITEM_MAT:
	{

		break;
	}
	case ITEM_SNOW:
	{
		break;
	}
	case ITEM_ICE:
	{
		break;
	}
	case ITEM_JET:
	{
		break;
	}
	case ITEM_SPBOX:
	{
		charactersInfo->players[sId].bGetSpBox = true;
		break;
	}
	default:
		break;
	}
}
void AMyPlayerController::InitPlayerSetting()
{
	if (!localPlayerCharacter) return;
	localPlayerCharacter->SetActorLocation(FVector(initInfo.X, initInfo.Y, initInfo.Z));
	localPlayerCharacter->iSessionId = initInfo.SessionId;
	localPlayerCharacter->SetCharacterMaterial(initInfo.iColor);
	//컨트롤러의 회전
	SetControlRotation(FRotator(0.0f, initInfo.Yaw, 0.0f));
	
	cCharacter* inputId = &charactersInfo->players[localPlayerCharacter->iSessionId];
	localPlayerCharacter->SetUserId(inputId->userId);

	char testId[MAX_NAME_SIZE];
	localPlayerCharacter->GetUserId(testId);

	FString str = testId;
	//MYLOG(Warning, TEXT("[PlayerCharacter] id : %s"), *str);

	bInitPlayerSetting = false;
}
void AMyPlayerController::UpdateTornado()
{
	UWorld* World = GetWorld();
	TArray<AActor*> SpawnedTornado;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATornado::StaticClass(), SpawnedTornado);
	if (!bTornado) return;
	for (auto sd : SpawnedTornado)
	{
		ATornado* tornado = Cast<ATornado>(sd);

		cCharacter* info = &charactersInfo->players[tornado->iSessionId];
		FVector CharacterLocation;
		CharacterLocation.X = info->X;
		CharacterLocation.Y = info->Y;
		CharacterLocation.Z = info->Z;

		//tornado->AddMovementInput(CharacterVelocity);
		//tornado->SetActorRotation(CharacterRotation);
		tornado->SetActorLocation(CharacterLocation);
		//MYLOG(Warning, TEXT("tornado %f, %f, %f"), info->X, info->Y, info->Z);

	}


}
bool AMyPlayerController::UpdateWorldInfo()
{
	UWorld* const world = GetWorld();
	if (world == nullptr)					return false;
	if (charactersInfo == nullptr)	return false;

	// 플레이어자신 체력, 사망업데이트
	UpdatePlayerInfo(charactersInfo->players[iSessionId]);
	UpdateTornado();

	// 스폰캐릭터들배열 하나 생성하고 월드에 있는 캐릭터들을 배열에 넣어주기
	TArray<AActor*> SpawnedCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyCharacter::StaticClass(), SpawnedCharacters);


	for (auto& Character_ : SpawnedCharacters)
	{
		//자신포함 모든플레이어
		AMyCharacter* player_ = Cast<AMyCharacter>(Character_);

		cCharacter* info = &charactersInfo->players[player_->iSessionId];
		if (!info->IsAlive) continue;
	
		if (info->bGetSpBox) {
			player_->GetSupplyBox();
			info->bGetSpBox = false;
		}

		if (info->bStartSnowBall) {
			player_->SnowBallAttack();
			info->bStartSnowBall = false;
		}

		if (info->bStartIceBall) {
			player_->IceballAttack();
			info->bStartIceBall = false;
		}

		if (info->bStartShotGun) {
			player_->ShotgunAttack();
			info->bStartShotGun = false;
		}

		if (info->bCancelSnowBall) {
			player_->CancelSnowBallAttack();
			info->bCancelSnowBall = false;
		}

		if (info->bCancelIceBall) {
			player_->CancelIceBallAttack();
			info->bCancelIceBall = false;
		}

		if (info->bEndSnowBall) {
			player_->ReleaseSnowball();
			info->bEndSnowBall = false;
			info->iCurrentSnowCount--;
		}

		if (info->bEndIceBall) {
			player_->ReleaseIceball();
			info->bEndIceBall = false;
			info->iCurrentIceCount--;
		}

		if (info->bEndShotGun) {
			MYLOG(Warning, TEXT("bEndShotGun"));
			player_->SpawnSnowballBomb();
			info->bEndShotGun = false;
			info->iCurrentSnowCount -= 5;
		}

		if (info->bStartUmb) {
			MYLOG(Warning, TEXT("bStartUmb"));
			player_->StartUmbrella();
			info->bStartUmb = false;
		}

		if (info->bEndUmb) {
			MYLOG(Warning, TEXT("bEndUmb"));
			//player_->ReleaseUmbrella();
			player_->GetAnim()->ResumeUmbrellaMontage();
			player_->CloseUmbrellaAnim();
			info->bEndUmb = false;
		}
		if (info->bSetJetSki) {
			MYLOG(Warning, TEXT("jetski"));
			player_->GetOnOffJetski();
			info->bSetJetSki = false;
		}

		//타플레이어 구별
		if (!player_ || player_->iSessionId == -1 || player_->iSessionId == iSessionId)
		{
			continue;
		}

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
			if (info->myState == ST_SNOWMAN)
			{
				info->iCurrentSnowCount = 0;
				Reset_Items(player_->iSessionId);
				player_->ChangeSnowman();
			}
		}
		else if (player_->IsSnowman())
		{
			if (info->myState == ST_ANIMAL)
			{
				Reset_Items(player_->iSessionId);
				player_->ChangeAnimal();
			}
		}
		// 캐릭터 속성 업데이트
		if (player_->iCurrentSnowballCount != info->iCurrentSnowCount)
		{
			player_->iCurrentSnowballCount = info->iCurrentSnowCount;
		}

	}
	return true;
}

void AMyPlayerController::UpdateNewPlayer()
{
	UWorld* const World = GetWorld();

	int size_ = newPlayers.Size();


	// 새로운 플레이어를 필드에 스폰
	FVector SpawnLocation_;
	SpawnLocation_.X = newplayer.get()->X;
	SpawnLocation_.Y = newplayer.get()->Y;
	SpawnLocation_.Z = newplayer.get()->Z;

	FRotator SpawnRotation;
	SpawnRotation.Yaw = newplayer.get()->Yaw;
	SpawnRotation.Pitch = 0.0f;
	SpawnRotation.Roll = 0.0f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.Name = FName(*FString(to_string(newplayer.get()->SessionId).c_str()));

	WhoToSpawn = AMyCharacter::StaticClass();
	AMyCharacter* SpawnCharacter = World->SpawnActor<AMyCharacter>(WhoToSpawn, SpawnLocation_, SpawnRotation, SpawnParams);
	SpawnCharacter->SpawnDefaultController();
	SpawnCharacter->iSessionId = newplayer.get()->SessionId;

	SpawnCharacter->SetCharacterMaterial(newplayer.get()->iColor);

	
	cCharacter* inputId = &charactersInfo->players[SpawnCharacter->iSessionId];
	SpawnCharacter->SetUserId(inputId->userId);

	char testId[MAX_NAME_SIZE];
	SpawnCharacter->GetUserId(testId);

	FString str = testId;
	//MYLOG(Warning, TEXT("[SpawnCharacter] id : %s"), *str);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("setuserId :%s"), SpawnCharacter->userId));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("info-> userId :%s"), info->userId));


// 필드의 플레이어 정보에 추가
	if (charactersInfo != nullptr)
	{
		cCharacter info;
		info.SessionId = newplayer.get()->SessionId;
		info.X = newplayer.get()->X;
		info.Y = newplayer.get()->Y;
		info.Z = newplayer.get()->Z;

		info.Yaw = newplayer.get()->Yaw;
		info.myState = ST_ANIMAL;

		charactersInfo->players[newplayer.get()->SessionId] = info;

	}

	newplayer = NULL;

	//MYLOG(Warning, TEXT("other player(id : %d) spawned."), newPlayers.front()->SessionId);

	//bNewPlayerEntered = false;
}

void AMyPlayerController::UpdateNewTornado()
{
	UWorld* const World = GetWorld();



	// 새로운 플레이어를 필드에 스폰
	FVector SpawnLocation_;
	SpawnLocation_.X = newtornado.get()->X;
	SpawnLocation_.Y = newtornado.get()->Y;
	SpawnLocation_.Z = newtornado.get()->Z;

	FRotator SpawnRotation;
	SpawnRotation.Yaw = newtornado.get()->Yaw;
	SpawnRotation.Pitch = 0.0f;
	SpawnRotation.Roll = 0.0f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.Name = FName(*FString(to_string(newtornado.get()->SessionId).c_str()));

	TornadoToSpawn = ATornado::StaticClass();
	ATornado* SpawnTornado = World->SpawnActor<ATornado>(TornadoToSpawn, SpawnLocation_, SpawnRotation, SpawnParams);
	SpawnTornado->SpawnDefaultController();
	SpawnTornado->iSessionId = newtornado.get()->SessionId;
	// 필드의 플레이어 정보에 추가
	if (charactersInfo != nullptr)
	{
		cCharacter info;
		info.SessionId = newtornado.get()->SessionId;
		info.X = newtornado.get()->X;
		info.Y = newtornado.get()->Y;
		info.Z = newtornado.get()->Z;
		info.Yaw = newtornado.get()->Yaw;
		info.myState = ST_TORNADO;
		charactersInfo->players[newtornado.get()->SessionId] = info;

	}

	newtornado = NULL;
	bTornado = true;

	MYLOG(Warning, TEXT("UpdateNewTornado"));

	//bNewPlayerEntered = false;
}

void AMyPlayerController::Reset_Items(int s_id)
{
	// 필드의 플레이어 정보에 추가
	if (charactersInfo != nullptr)
	{
		charactersInfo->players[s_id].iCurrentSnowCount = 0;
		//charactersInfo->players[s_id].iCurrentMatchCount = 0;
		//charactersInfo->players[s_id].bHasUmbrella = false;
		//charactersInfo->players[s_id].bHasBag = false;;

	}
	/*iCurrentSnowballCount = 0;
	iMaxSnowballCount = iOriginMaxSnowballCount;
	iCurrentMatchCount = 0;
	iMaxMatchCount = iOriginMaxMatchCount;
	bHasUmbrella = false;
	bHasBag = false;*/

	//UpdateUI(UICategory::AllOfUI);
}

void AMyPlayerController::SendPlayerInfo(int input)
{
	if (!localPlayerCharacter) return;
	FVector MyCameraLocation;
	FRotator MyCameraRotation;
	float fspeed;
	switch (input)
	{
	case COMMAND_MOVE: {
		auto loc = localPlayerCharacter->GetActorLocation();
		float fNewYaw = localPlayerCharacter->GetActorRotation().Yaw;		//yaw 값만 필요함
		auto vel = localPlayerCharacter->GetVelocity();
		float dir = localPlayerCharacter->GetAnim()->GetDirection();
		mySocket->Send_MovePacket(iSessionId, loc, fNewYaw, vel, dir);
		break;
	}
	case COMMAND_SNOWBALL: {
		mySocket->Send_AttackPacket(iSessionId, BULLET_SNOWBALL);
		break;
	}
	case COMMAND_ICEBALL: {
		mySocket->Send_AttackPacket(iSessionId, BULLET_ICEBALL);
		break;
	}
	case COMMAND_SHOTGUN: {
		mySocket->Send_GunAttackPacket(iSessionId);
		break;
	}
	case COMMAND_THROW_SB: {
		if (IsValid(localPlayerCharacter->snowball))
		{
			FVector MySnowBallLocation = localPlayerCharacter->snowball->GetActorLocation();
			localPlayerCharacter->GetActorEyesViewPoint(MyCameraLocation, MyCameraRotation);
			fspeed = localPlayerCharacter->Getfspeed();
			mySocket->Send_Throw_Packet(iSessionId, MySnowBallLocation, MyCameraRotation, BULLET_SNOWBALL, fspeed);
		}
		break;
	}
	case COMMAND_THROW_IB: {
		if (IsValid(localPlayerCharacter->iceball))
		{
			FVector MyIceBallLocation = localPlayerCharacter->iceball->GetActorLocation();
			localPlayerCharacter->GetActorEyesViewPoint(MyCameraLocation, MyCameraRotation);
			fspeed = localPlayerCharacter->Getfspeed();
			mySocket->Send_Throw_Packet(iSessionId, MyIceBallLocation, MyCameraRotation, BULLET_ICEBALL, fspeed);
		}
		break;
	}
	case COMMAND_CANCEL_SB: {
		mySocket->Send_Cancel_Packet(iSessionId, BULLET_SNOWBALL);
		break;
	}
	case COMMAND_CANCEL_IB: {
		mySocket->Send_Cancel_Packet(iSessionId, BULLET_ICEBALL);
		break;
	}
	case COMMAND_GUNFIRE: {
		localPlayerCharacter->GetActorEyesViewPoint(MyCameraLocation, MyCameraRotation);
		mySocket->Send_GunFire_Packet(iSessionId, MyCameraLocation, MyCameraRotation);
		break;
	}
	case COMMAND_DAMAGE: {
		mySocket->Send_DamagePacket();
		break;
	}
	case COMMAND_MATCH: {
		mySocket->Send_MatchPacket();
		break;
	}
	case COMMAND_UMB_START: {
		mySocket->Send_UmbPacket(false);
		break;
	}
	case COMMAND_UMB_END: {
		mySocket->Send_UmbPacket(true);
		break;
	}
	default:
		break;
	}
}

void AMyPlayerController::SendTeleportInfo(int input)
{
	/*if (!localPlayerCharacter)
		return;

	if (input == TEL_FIRE)

	else if (input == TEL_BRIDGE)

	else if (input == TEL_TOWER)

	else if (input == TEL_ICE)
	*/
}

void AMyPlayerController::SendCheatInfo(int input)
{
	if (!localPlayerCharacter)
		return;
	auto loc = localPlayerCharacter->GetActorLocation();
	float fNewYaw = localPlayerCharacter->GetActorRotation().Yaw;		//yaw 값만 필요함
	auto vel = localPlayerCharacter->GetVelocity();
	FVector MyCameraLocation;
	FRotator MyCameraRotation;
	localPlayerCharacter->GetActorEyesViewPoint(MyCameraLocation, MyCameraRotation);
	float dir = localPlayerCharacter->GetAnim()->GetDirection();

	if (input == CHEAT_HP_UP)
		mySocket->Send_ChatPacket(CHEAT_HP_UP);
	else if (input == CHEAT_HP_DOWN)
		mySocket->Send_ChatPacket(CHEAT_HP_DOWN);
	else if (input == CHEAT_SNOW_PLUS)
		mySocket->Send_ChatPacket(CHEAT_SNOW_PLUS);
	else if (input == CHEAT_ICE_PLUS)
		mySocket->Send_ChatPacket(CHEAT_ICE_PLUS);

}

//플레이어 정보 업데이트
void AMyPlayerController::UpdatePlayerInfo(cCharacter& info)
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
		if (bIsBone) {
			player_->UpdateTemperatureState();
			bIsBone = false;
		}
		//눈사람 변화
		if (player_->IsSnowman())
		{
			if (info.myState == ST_ANIMAL)
			{
				Reset_Items(player_->iSessionId);
				info.HealthValue = player_->iBeginSlowHP;
				player_->ChangeAnimal();
			}
		}
		else if (!player_->IsSnowman())
		{
			if (info.myState == ST_SNOWMAN)
			{
				Reset_Items(player_->iSessionId);
				info.HealthValue = player_->iMinHP;
				player_->ChangeSnowman();
			}
		}
		// 캐릭터 속성 업데이트
		if (player_->iCurrentHP != info.HealthValue)
		{
			//MYLOG(Warning, TEXT("Player damaged hp: %d"), info.HealthValue);
			player_->iCurrentHP = info.HealthValue;
			CallDelegateUpdateHP();

		}
		// 캐릭터 속성 업데이트
		if (player_->iCurrentSnowballCount != info.iCurrentSnowCount)
		{
			//MYLOG(Warning, TEXT("Player damaged hp: %d"), info.HealthValue);
			player_->iCurrentSnowballCount = info.iCurrentSnowCount;
			CallDelegateUpdateCurrentSnowballCount();
		}
		// 캐릭터 속성 업데이트
		if (player_->iCurrentIceballCount != info.iCurrentIceCount)
		{
			//MYLOG(Warning, TEXT("Player damaged hp: %d"), info.HealthValue);
			player_->iCurrentIceballCount = info.iCurrentIceCount;
			CallDelegateUpdateCurrentIceballCount();
		}

	}
}

//void AMyPlayerController::SendFarming(int item_no)
//{
//		mySocket->Send_ItemPacket(item_no);
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
	mySocket->Send_ReadyPacket();
	UE_LOG(LogTemp, Warning, TEXT("PlayerReady"));
	bIsReady = true;
#ifdef SINGLEPLAY_DEBUG
	StartGame();	// 디버깅용 - 레디버튼 누르면 startgame 호출
#endif
}

void AMyPlayerController::PlayerUnready()
{
	// 서버에 언레디했다고 전송
	UE_LOG(LogTemp, Warning, TEXT("PlayerUnready"));
	bIsReady = false;
}

void AMyPlayerController::StartGame()
{
	if (!bInGame) {
		UE_LOG(LogTemp, Warning, TEXT("StartGame"));
		bInGame = true;
		readyUI->RemoveFromParent();	// ReadyUI 제거
		LoadCharacterUI(); // CharacterUI 띄우기
		// 실행시 클릭없이 바로 조작
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
		auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		m_Player->UpdateTemperatureState();
	}
	// 게임 시작되면 실행시킬 코드들 작성
}

void AMyPlayerController::LoadCharacterUI()
{
	if (characterUIClass)
	{
		characterUI = CreateWidget<UUserWidget>(GetWorld(), characterUIClass);
		if (characterUI)
		{
			characterUI->AddToViewport();
			CallDelegateUpdateAllOfUI();	// 모든 캐릭터 ui 갱신
		}
	}
}

// 게임 종료 시 결과창 ui 띄우기 (승자 id 인자로 받아서 승자, 패자 ui 다르게 뜨도록)
void AMyPlayerController::LoadGameResultUI(int winnerSessionId)
{	// remove character ui 
	if (gameResultUIClass)
	{
		gameResultUI = CreateWidget<UUserWidget>(GetWorld(), gameResultUIClass);
		if (gameResultUI)
		{
			characterUI->RemoveFromParent();	// character ui 제거
			gameResultUI->AddToViewport();		// game result ui 띄우기

			bool bIsWinner = (iSessionId == winnerSessionId) ? true : false;
			CallDelegateUpdateGameResult(bIsWinner);

			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
			bShowMouseCursor = true;
		}
	}
}

void AMyPlayerController::CallDelegateUpdateAllOfUI()
{
	CallDelegateUpdateHP();
	CallDelegateUpdateCurrentSnowballCount();
	CallDelegateUpdateCurrentIceballCount();
	CallDelegateUpdateCurrentMatchCount();
	CallDelegateUpdateMaxSnowIceballAndMatchCount();
	CallDelegateUpdateHasUmbrella();
	CallDelegateUpdateHasBag();

	CallDelegateUpdateSelectedItem();
	CallDelegateUpdateSelectedProjectile();
	CallDelegateUpdateSelectedWeapon();
}

void AMyPlayerController::CallDelegateUpdateHP()
{
	if (!characterUI) return;	// CharacterUI가 생성되기 전이면 갱신 x

	if (FuncUpdateHP.IsBound() == true) FuncUpdateHP.Broadcast(localPlayerCharacter->iCurrentHP);	// 델리게이트 호출

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update hp %d"), localPlayer->iCurrentHP);
}

void AMyPlayerController::CallDelegateUpdateCurrentSnowballCount()
{
	if (!characterUI) return;

	if (FuncUpdateCurrentSnowballCount.IsBound() == true) FuncUpdateCurrentSnowballCount.Broadcast(localPlayerCharacter->iCurrentSnowballCount);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update current snowball count %d"), localPlayerCharacter->iCurrentSnowballCount);
}

void AMyPlayerController::CallDelegateUpdateCurrentIceballCount()
{
	if (!characterUI) return;

	if (FuncUpdateCurrentIceballCount.IsBound() == true) FuncUpdateCurrentIceballCount.Broadcast(localPlayerCharacter->iCurrentIceballCount);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update current iceball count %d"), localPlayerCharacter->iCurrentIceballCount);
}

void AMyPlayerController::CallDelegateUpdateCurrentMatchCount()
{
	if (!characterUI) return;

	if (FuncUpdateCurrentMatchCount.IsBound() == true) FuncUpdateCurrentMatchCount.Broadcast(localPlayerCharacter->iCurrentMatchCount);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update current match count %d"), localPlayerCharacter->iCurrentMatchCount);
}

void AMyPlayerController::CallDelegateUpdateMaxSnowIceballAndMatchCount()
{
	if (!characterUI) return;

	if (FuncUpdateMaxSnowIceballAndMatchCount.IsBound() == true)
		FuncUpdateMaxSnowIceballAndMatchCount.Broadcast(
			localPlayerCharacter->iMaxSnowballCount, localPlayerCharacter->iMaxIceballCount, localPlayerCharacter->iMaxMatchCount);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update max snowball count %d, max iceball count %d, max match count %d"), 
	//	localPlayerCharacter->iMaxSnowballCount, localPlayerCharacter->iMaxIceballCount, localPlayerCharacter->iMaxMatchCount);
}

void AMyPlayerController::CallDelegateUpdateHasUmbrella()
{
	if (!characterUI) return;

	if (FuncUpdateHasUmbrella.IsBound() == true) FuncUpdateHasUmbrella.Broadcast(localPlayerCharacter->bHasUmbrella);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update has umbrella %d"), localPlayerCharacter->bHasUmbrella);
}

void AMyPlayerController::CallDelegateUpdateHasBag()
{
	if (!characterUI) return;

	if (FuncUpdateHasBag.IsBound() == true) FuncUpdateHasBag.Broadcast(localPlayerCharacter->bHasBag);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update has bag %d"), localPlayerCharacter->bHasBag);
}

void AMyPlayerController::CallDelegateUpdateHasShotgun()
{
	if (!characterUI) return;

	if (FuncUpdateHasShotgun.IsBound() == true) FuncUpdateHasShotgun.Broadcast(localPlayerCharacter->bHasShotgun);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update has shotgun %d"), localPlayerCharacter->bHasShotgun);
}

void AMyPlayerController::CallDelegateUpdateIsFarmingSnowdrift()
{
	if (!characterUI) return;

	if (FuncUpdateIsFarmingSnowdrift.IsBound() == true) FuncUpdateIsFarmingSnowdrift.Broadcast(localPlayerCharacter->GetIsFarming());

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update is farming snowdrift %d"), localPlayerCharacter->GetIsFarming());
}

void AMyPlayerController::CallDelegateUpdateSnowdriftFarmDuration(float farmDuration)
{
	if (!characterUI) return;

	if (FuncUpdateSnowdriftFarmDuration.IsBound() == true) FuncUpdateSnowdriftFarmDuration.Broadcast(farmDuration);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update snowdrift farm duration %f"), farmDuration);
}

void AMyPlayerController::CallDelegateUpdateSelectedItem()
{
	if (!characterUI) return;

	if (FuncUpdateSelectedItem.IsBound() == true) FuncUpdateSelectedItem.Broadcast(localPlayerCharacter->iSelectedItem);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update selected item %d"), localPlayerCharacter->iSelectedItem);
}

void AMyPlayerController::CallDelegateUpdateSelectedProjectile()
{
	if (!characterUI) return;

	if (FuncUpdateSelectedProjectile.IsBound() == true) FuncUpdateSelectedProjectile.Broadcast(localPlayerCharacter->iSelectedProjectile);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update selected projectile %d"), localPlayerCharacter->iSelectedProjectile);
}

void AMyPlayerController::CallDelegateUpdateSelectedWeapon()
{
	if (!characterUI) return;

	if (FuncUpdateSelectedWeapon.IsBound() == true) FuncUpdateSelectedWeapon.Broadcast(localPlayerCharacter->iSelectedWeapon);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update selected weapon %d"), localPlayerCharacter->iSelectedWeapon);
}

void AMyPlayerController::CallDelegateUpdateKillLog(int attacker, int victim, int cause)
{	// 추위에 의한 죽음은 attacker id = -2
	if (!characterUI) return;

	char* userId;
	if (attacker != -2)
		userId = charactersInfo->players[attacker].userId;
	else
		userId = "none";
	FString attackerUserId = userId;
	userId = charactersInfo->players[victim].userId;
	FString victimUserId = userId;

	int killLogType;
	if (attacker == iSessionId)
		killLogType = KillLogType::Attacker;
	else if (victim == iSessionId)
		killLogType = KillLogType::Victim;
	else
		killLogType = KillLogType::None;


	if (FuncUpdateKillLog.IsBound() == true) FuncUpdateKillLog.Broadcast(attackerUserId, victimUserId, cause, killLogType);

	if (cause == CauseOfDeath::DeathBySnowman && (attacker != victim))
		CallDelegateUpdateKillLog(attacker, attacker, cause);

	//UE_LOG(LogTemp, Warning, TEXT("call delegate update kill log %d %d %d"), attacker, victim, cause);
}

void AMyPlayerController::CallDelegateUpdateGameResult(bool isWinner)
{
	if (!gameResultUI) return;

	if (FuncUpdateGameResult.IsBound() == true) FuncUpdateGameResult.Broadcast(isWinner);
}

void AMyPlayerController::FixRotation()
{
	float pitch, yaw, roll;
	UKismetMathLibrary::BreakRotator(GetControlRotation(), roll, pitch, yaw);
	pitch = UKismetMathLibrary::ClampAngle(pitch, -15.0f, 30.0f);
	FRotator newRotator = UKismetMathLibrary::MakeRotator(roll, pitch, yaw);
	SetControlRotation(newRotator);
}

void AMyPlayerController::LoadLoginUI()
{
	if (loginUIClass)
	{
		loginUI = CreateWidget<UUserWidget>(GetWorld(), loginUIClass);
		if (loginUI)
		{
			loginUI->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
			bShowMouseCursor = true;
		}
	}
}

void AMyPlayerController::BtnCreateAccount(FString id, FString pw)
{
	//UE_LOG(LogTemp, Warning, TEXT("CREATE id : %s, pw : %s"), *id, *pw);

	// id, pw를 모두 입력하지 않은 경우 return
	if (id.Len() == 0 && pw.Len() == 0)
	{
		loginInfoText = TEXT("Input ID and PW");
		return;
	}
	else if (id.Len() == 0)
	{
		loginInfoText = TEXT("Input ID");
		return;
	}
	else if (pw.Len() == 0)
	{
		loginInfoText = TEXT("Input PW");
		return;
	}

	g_socket->Send_Create_ID_Packet(TCHAR_TO_UTF8(*id), TCHAR_TO_UTF8(*pw));
}

void AMyPlayerController::BtnLogin(FString id, FString pw)
{
	//UE_LOG(LogTemp, Warning, TEXT("LOGIN id : %s, pw : %s"), *id, *pw);


	// 디버깅용 - id, pw에 입력 x인 경우 게임 플레이 되도록
	if (id.Len() == 0 && pw.Len() == 0)
	{
		id = FString(TEXT("testuser"));
		pw = FString(TEXT("testuser"));
		g_socket->Send_LoginPacket(TCHAR_TO_UTF8(*id), TCHAR_TO_UTF8(*pw));
	}
	else
	{
		if (id.Len() == 0)
		{
			loginInfoText = TEXT("Input ID");
			return;
		}
		if (pw.Len() == 0)
		{
			loginInfoText = TEXT("Input PW");
			return;
		}
		g_socket->Send_LoginPacket(TCHAR_TO_UTF8(*id), TCHAR_TO_UTF8(*pw));
	}
}

void AMyPlayerController::DeleteLoginUICreateReadyUI()
{
	loginUI->RemoveFromParent();	// LoginUI 제거
	LoadReadyUI(); // ReadyUI 띄우기
}

void AMyPlayerController::SpawnSupplyBox(float x, float y, float z)
{
	if (supplyboxClass)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			FVector vector = FVector(x, y, z);
			FTransform transform = FTransform(FRotator(0.0f), vector, FVector(1.0f));
			World->SpawnActor<AActor>(supplyboxClass, transform, SpawnParams);
		}
	}
}
