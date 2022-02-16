// Fill out your copyright notice in the Description page of Project Settings.

#include "ABPlayerController.h"

AABPlayerController::AABPlayerController()
{
	SessionId = FMath::RandRange(0, 10000);

	// 서버와 연결
	Socket = ClientSocket::GetSingleton();
	Socket->InitSocket();
	bIsConnected = Socket->Connect("127.0.0.1", 8000);
	if (bIsConnected)
	{
		ABLOG(Warning, TEXT("IOCP Server connect success!"));
		Socket->SetPlayerController(this);
	}
}

AABPlayerController::~AABPlayerController()
{

}

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPlayerController::OnPossess(APawn *aPawn)
{
	ABLOG_S(Warning);
	Super::OnPossess(aPawn);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}
