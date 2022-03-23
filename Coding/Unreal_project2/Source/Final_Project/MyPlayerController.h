// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <thread>
#include "Final_Project.h"
#include "MyCharacter.h"
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
	ClientSocket* _cs;
	int*  _session_Id;
	void UpdatePlayerInfo(int input);
protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	

	void Disconnect();

private:
	//ClientSocket cs;
	//HANDLE h_iocp;
};
