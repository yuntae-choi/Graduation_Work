// Fill out your copyright notice in the Description page of Project Settings.

#include "ABPlayerController.h"

AABPlayerController::AABPlayerController()
{
	// 서버와 연결
	bIsConnected = Socket->Connect();
	if (bIsConnected)
	{
		ABLOG(Warning, TEXT("IOCP Server connect success!"));
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
