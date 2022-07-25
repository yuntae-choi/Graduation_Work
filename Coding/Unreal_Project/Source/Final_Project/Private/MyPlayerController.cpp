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
	victory_player.store(-1);
	bInGame = false;
	PrimaryActorTick.bCanEverTick = true;
	bear_cnt = 0;
	snowman_cnt = 0;
	
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

void AMyPlayerController::SetAttack(const int s_id, int at_type)
{
	UWorld* World = GetWorld();
	switch (at_type)
	{
	case ATTACK_SNOWBALL: {
		charactersInfo->players[s_id].Start_SnowBall = true;
		break;
	}
	case ATTACK_ICEBALL: {
		charactersInfo->players[s_id].Start_IceBall = true;
		break;
	}
	case ATTACK_SHOTGUN: {
		charactersInfo->players[s_id].Start_ShotGun = true;
		break;
	}
	case END_SNOWBALL: {
		charactersInfo->players[s_id].End_SnowBall = true;
		break;
	}
	case END_ICEBALL: {
		charactersInfo->players[s_id].End_IceBall = true;
		break;
	}
	case END_SHOTGUN: {
		charactersInfo->players[s_id].End_ShotGun = true;
		break;
	}
	case CANCEL_SNOWBALL: {
		charactersInfo->players[s_id].Cancel_SnowBall = true;
		break;
	}
	case CANCEL_ICEBALL: {
		charactersInfo->players[s_id].Cancel_IceBall = true;
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
			charactersInfo->players[s_id].start_umb = true;
		}
		else {
			charactersInfo->players[s_id].end_umb = true;
		}
		break;
	}
	case ITEM_JET: {
		charactersInfo->players[s_id].SET_JET_SKI = true;
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

void AMyPlayerController::get_item(int itemType)
{

	auto player_ = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!player_)
		return;
	//player_->GetItem(itemType);
}
void AMyPlayerController::InitPlayerSetting()
{
	if (!localPlayerCharacter) return;
	localPlayerCharacter->SetActorLocation(FVector(initInfo.X, initInfo.Y, initInfo.Z));
	localPlayerCharacter->iSessionId = initInfo.SessionId;

	//컨트롤러의 회전
	SetControlRotation(FRotator(0.0f, initInfo.Yaw, 0.0f));
	if (itonardoId == 0)
		localPlayerCharacter->SetCharacterMaterial(iSessionId - 1);
	else
		localPlayerCharacter->SetCharacterMaterial(iSessionId);
	bInitPlayerSetting = false;
}
void AMyPlayerController::UpdateTornado()
{
	UWorld* World = GetWorld();
	TArray<AActor*> SpawnedTornado;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATornado::StaticClass(), SpawnedTornado);
	if (itonardoId == -1) return;
	for (auto sd : SpawnedTornado)
	{
		ATornado* tornado = Cast<ATornado>(sd);

		cCharacter* info = &charactersInfo->players[itonardoId];
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

		if (info->Start_SnowBall) { 
			player_->SnowBallAttack();
			info->Start_SnowBall = false;
		}
		
		if (info->Start_IceBall) {
			player_->IceballAttack();
			info->Start_IceBall = false;
		}
		
		if (info->Start_ShotGun) {
			player_->ShotgunAttack();
			info->Start_ShotGun = false;
		}

		if (info->Cancel_SnowBall) {
			player_->Cancel_SnowBallAttack();
			info->Cancel_SnowBall = false;
		}

		if (info->Cancel_IceBall) {
			player_->Cancel_IceBallAttack();
			info->Cancel_IceBall = false;
		}

		if (info->End_SnowBall) {
			player_->ReleaseSnowball();
			info->End_SnowBall = false;
			info->current_snow_count--;
		}

		if (info->End_IceBall) {
			player_->ReleaseIceball();
			info->End_IceBall = false;
			info->current_ice_count--;
		}

		if (info->End_ShotGun) {
			MYLOG(Warning, TEXT("End_ShotGun"));
			player_->SpawnSnowballBomb();
			info->End_ShotGun = false;
			info->current_snow_count-=5;
		}

		if (info->start_umb) {
			MYLOG(Warning, TEXT("start_umb"));
			player_->StartUmbrella();
			info->start_umb = false;
		}

		if (info->end_umb) {
			MYLOG(Warning, TEXT("end_umb"));
			//player_->ReleaseUmbrella();
			player_->GetAnim()->ResumeUmbrellaMontage();
			player_->CloseUmbrellaAnim();
			info->end_umb = false;
		}
		if (info->SET_JET_SKI) {
			MYLOG(Warning, TEXT("jetski"));
			player_->GetOnOffJetski();
			info->SET_JET_SKI = false;
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
			if (info->My_State == ST_SNOWMAN)
			{
				info->current_snow_count = 0;
				Reset_Items(player_->iSessionId);
				player_->ChangeSnowman();
			}
		}
		else if (player_->IsSnowman())
		{
			if (info->My_State == ST_ANIMAL)
			{
				Reset_Items(player_->iSessionId);
				player_->ChangeAnimal();
			}
		}
		// 캐릭터 속성 업데이트
		if (player_->iCurrentSnowballCount != info->current_snow_count)
		{
			player_->iCurrentSnowballCount = info->current_snow_count;
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

	if (newplayer.get()->SessionId != itonardoId)
	{
		WhoToSpawn = AMyCharacter::StaticClass();
		AMyCharacter* SpawnCharacter = World->SpawnActor<AMyCharacter>(WhoToSpawn, SpawnLocation_, SpawnRotation, SpawnParams);
		SpawnCharacter->SpawnDefaultController();
		SpawnCharacter->iSessionId = newplayer.get()->SessionId;
		if(itonardoId == 0) 
			SpawnCharacter->SetCharacterMaterial(SpawnCharacter->iSessionId -1);
		else
			SpawnCharacter->SetCharacterMaterial(SpawnCharacter->iSessionId);
	}
	else if(newplayer.get()->SessionId == itonardoId)
	{
		if (itonardoId == -1) return;

		TornadoToSpawn = ATornado::StaticClass();
		ATornado* SpawnTornado = World->SpawnActor<ATornado>(TornadoToSpawn, SpawnLocation_, SpawnRotation, SpawnParams);
		SpawnTornado -> SpawnDefaultController();
	}
	// 필드의 플레이어 정보에 추가
	if (charactersInfo != nullptr)
	{
		cCharacter info;
		info.SessionId = newplayer.get()->SessionId;
		info.X = newplayer.get()->X;
		info.Y = newplayer.get()->Y;
		info.Z = newplayer.get()->Z;

		info.Yaw = newplayer.get()->Yaw;

		charactersInfo->players[newplayer.get()->SessionId] = info;
	}

	newplayer = NULL;

	//MYLOG(Warning, TEXT("other player(id : %d) spawned."), newPlayers.front()->SessionId);

	//bNewPlayerEntered = false;
}


void AMyPlayerController::Reset_Items(int s_id)
{
	// 필드의 플레이어 정보에 추가
	if (charactersInfo != nullptr)
	{
		charactersInfo->players[s_id].current_snow_count = 0;
		//charactersInfo->players[s_id].current_match_count = 0;
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
		if (bisBone) { 
			player_->UpdateTemperatureState();
			bisBone = false;
		}
		//눈사람 변화
		if (player_->IsSnowman())
		{
			if (info.My_State == ST_ANIMAL)
			{
				Reset_Items(player_->iSessionId);
				info.HealthValue = player_->iBeginSlowHP;
				player_->ChangeAnimal();
			}
		}
		else if (!player_->IsSnowman())
		{
			if (info.My_State == ST_SNOWMAN)
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
		if (player_->iCurrentSnowballCount != info.current_snow_count)
		{
			//MYLOG(Warning, TEXT("Player damaged hp: %d"), info.HealthValue);
			player_->iCurrentSnowballCount = info.current_snow_count;
			CallDelegateUpdateCurrentSnowballCount();
		}
		// 캐릭터 속성 업데이트
		if (player_->iCurrentIceballCount != info.current_ice_count)
		{
			//MYLOG(Warning, TEXT("Player damaged hp: %d"), info.HealthValue);
			player_->iCurrentIceballCount = info.current_ice_count;
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