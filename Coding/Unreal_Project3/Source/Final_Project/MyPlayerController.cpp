// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ClientSocket.h"

#pragma comment(lib, "ws2_32.lib")


AMyPlayerController::AMyPlayerController()
{
	
	cs = ClientSocket::GetSingleton();
	cs->h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(cs->_socket), cs->h_iocp, 0, 0);
	int ret = cs->Connect();
	if (ret) 
		{
			UE_LOG(LogClass, Log, TEXT("IOCP Server connect success!"));
			cs->SetPlayerController(this);
		}
	

	PrimaryActorTick.bCanEverTick = true;
}


void AMyPlayerController::BeginPlay()
{
	
	cs->StartListen();
	//Super::BeginPlay(); //게임 종료가 안됨

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	cs->CloseSocket();
	cs->StopListen();
	//Super::EndPlay(EndPlayReason);
}

void AMyPlayerController::Tick(float DeltaTime)
{
	
	Super::Tick(DeltaTime);
	cs->ReadyToSend_MovePacket(1);
	
}

