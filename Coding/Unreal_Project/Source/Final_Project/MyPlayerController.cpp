// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ClientSocket.h"

#pragma comment(lib, "ws2_32.lib")


AMyPlayerController::AMyPlayerController()
{

	_cs = ClientSocket::GetSingleton();
	/*_cs->h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_cs->_socket), _cs->h_iocp, 0, 0);
	*///int ret = _cs->Connect();
	//if (ret)
	//{
		//UE_LOG(LogClass, Log, TEXT("IOCP Server connect success!"));
		_cs->SetPlayerController(this);
		
	//}


	PrimaryActorTick.bCanEverTick = true;

	bNewPlayerEntered = false;
}


void AMyPlayerController::BeginPlay()
{

	//Super::BeginPlay(); //게임 종료가 안됨

	/*auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	_session_Id = &m_Player->_SessionId;
	auto MyLocation = m_Player->GetActorLocation();
	auto MyRotation = m_Player->GetActorRotation();*/

	_cs->StartListen();
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	_cs->CloseSocket();
	_cs->StopListen();
	//Super::EndPlay(EndPlayReason);
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bNewPlayerEntered)
		UpdateNewPlayer();

	UpdateRotation();
}


void AMyPlayerController::UpdatePlayerInfo(int input)
{
	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	_my_session_id = m_Player->_SessionId;
	auto MyLocation = m_Player->GetActorLocation();
	auto MyRotation = m_Player->GetActorRotation();
	if (input == COMMAND_MOVE)
		_cs->ReadyToSend_MovePacket(_my_session_id, MyLocation.X, MyLocation.Y, MyLocation.Z);
	else if (input == COMMAND_ATTACK)
		_cs->ReadyToSend_AttackPacket();

}

void AMyPlayerController::UpdatePlayerS_id(int _s_id)
{
	_my_session_id = _s_id;
	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	m_Player->_SessionId = _s_id;

	

}

void AMyPlayerController::RecvNewPlayer(int sessionID, float x, float y, float z)
{
	MYLOG(Warning, TEXT("recv"));
	bNewPlayerEntered = true;
	_other_session_id = sessionID;
	_other_x = x;
	_other_y = y;
	_other_z = z;
}

void AMyPlayerController::UpdateNewPlayer()
{
	UWorld* const world = GetWorld();

	// 새로운 플레이어가 자기 자신이면 무시
	if (_other_session_id == _my_session_id)
	{
		bNewPlayerEntered = false;
		return;
	}

	// 새로운 플레이어를 필드에 스폰
	FVector SpawnLocation_;
	SpawnLocation_.X = _other_x;
	SpawnLocation_.Y = _other_y;
	SpawnLocation_.Z = _other_z;

	//FRotator SpawnRotation;
	//SpawnRotation.Yaw = NewPlayer->Yaw;
	//SpawnRotation.Pitch = NewPlayer->Pitch;
	//SpawnRotation.Roll = NewPlayer->Roll;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.Name = FName(*FString(to_string(_other_session_id).c_str()));

	AMyCharacter* SpawnCharacter = world->SpawnActor<AMyCharacter>(WhoToSpawn, SpawnLocation_, FRotator::ZeroRotator, SpawnParams);
	SpawnCharacter->SpawnDefaultController();
	SpawnCharacter->_SessionId = _other_session_id;
	//SpawnCharacter->HealthValue = NewPlayer->HealthValue;

	//// 필드의 플레이어 정보에 추가
	//if (CharactersInfo != nullptr)
	//{
	//	cCharacter player;
	//	player.SessionId = NewPlayer->SessionId;
	//	player.X = NewPlayer->X;
	//	player.Y = NewPlayer->Y;
	//	player.Z = NewPlayer->Z;

	//	player.Yaw = NewPlayer->Yaw;
	//	player.Pitch = NewPlayer->Pitch;
	//	player.Roll = NewPlayer->Roll;

	//	player.VX = NewPlayer->VX;
	//	player.VY = NewPlayer->VY;
	//	player.VZ = NewPlayer->VZ;

	//	player.IsAlive = NewPlayer->IsAlive;
	//	player.HealthValue = NewPlayer->HealthValue;
	//	player.IsAttacking = NewPlayer->IsAttacking;

	//	CharactersInfo->players[NewPlayer->SessionId] = player;
	//	}

	bNewPlayerEntered = false;
}

void AMyPlayerController::UpdateRotation()
{
	float pitch, yaw, roll;
	UKismetMathLibrary::BreakRotator(GetControlRotation(), roll, pitch, yaw);
	pitch = UKismetMathLibrary::ClampAngle(pitch, -15.0f, 30.0f);
	FRotator newRotator = UKismetMathLibrary::MakeRotator(roll, pitch, yaw);
	SetControlRotation(newRotator);
}