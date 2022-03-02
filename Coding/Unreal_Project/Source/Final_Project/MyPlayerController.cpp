// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

AMyPlayerController::AMyPlayerController()
{
	// 서버와 연결
	bIsConnected = Socket->Connect();
	if (bIsConnected)
	{
		MYLOG(Warning, TEXT("IOCP Server connect success!"));
	}
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}
