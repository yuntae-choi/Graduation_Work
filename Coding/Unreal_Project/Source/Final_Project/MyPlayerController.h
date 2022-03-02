// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "GameFramework/PlayerController.h"
#include "ClientSocket.h"
#include "MyPlayerController.generated.h"

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

	ClientSocket*		Socket;			// 辑滚客 立加且 家南
	bool				bIsConnected;	// 辑滚客 立加 蜡公
};
