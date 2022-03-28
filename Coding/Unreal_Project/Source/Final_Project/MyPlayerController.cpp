// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ClientSocket.h"

#pragma comment(lib, "ws2_32.lib")


AMyPlayerController::AMyPlayerController()
{

	_cs = ClientSocket::GetSingleton();
	_cs->h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_cs->_socket), _cs->h_iocp, 0, 0);
	int ret = _cs->Connect();
	if (ret)
	{
		UE_LOG(LogClass, Log, TEXT("IOCP Server connect success!"));
		_cs->SetPlayerController(this);
	}


	PrimaryActorTick.bCanEverTick = true;
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
}


void AMyPlayerController::UpdatePlayerInfo(int input)
{
	UE_LOG(LogClass, Log, TEXT("move!"));

	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	_session_Id = &m_Player->_SessionId;
	auto MyLocation = m_Player->GetActorLocation();
	auto MyRotation = m_Player->GetActorRotation();
	if (input == COMMAND_MOVE)
		_cs->ReadyToSend_MovePacket(MyLocation.X, MyLocation.Y, MyLocation.Z);
	else if (input == COMMAND_ATTACK)
		_cs->ReadyToSend_AttackPacket();

}

