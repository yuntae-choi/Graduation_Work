// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include "Final_Project.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

#pragma comment(lib, "ws2_32.lib")

/**
 * 
 */
UCLASS()
class FINAL_PROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMyPlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
};
