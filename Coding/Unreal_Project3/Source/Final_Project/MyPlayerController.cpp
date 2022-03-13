// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"

#pragma comment(lib, "ws2_32.lib")

DWORD WINAPI ClientMain(LPVOID arg);

AMyPlayerController::AMyPlayerController()
{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	cs.Connect();

	cs.ReadyToSend_LoginPacket();
	cs.ReadyToSend_MovePacket('1');

	//CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

DWORD WINAPI ClientMain(LPVOID arg)
{

	return 0;
}
