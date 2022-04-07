// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <thread>
#include "Final_Project.h"
#include "MyCharacter.h"
#include "GameFramework/PlayerController.h"
#include "ClientSocket.h"
#include "Kismet/KismetMathLibrary.h"

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

	void RecvNewPlayer(int sessionID, float x, float y, float z);
	void UpdateNewPlayer();

	//void UpdateNewPlayer(int new_s_id, float new_x, float new_y, float new_z);
	void UpdatePlayerInfo(int input);
	void UpdatePlayerS_id(int _s_id);

	//void UpdateRotation();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	ClientSocket* _cs;

	int32 _my_session_id;
	float _my_x;
	float _my_y;
	float _my_z;
	int32 _other_session_id;
	float _other_x;
	float _other_y;
	float _other_z;

	//UPROPERTY(EditAnywhere, Category = "Spawning")
	//TSubclassOf<class AMyCharacter> WhoToSpawn;

protected:
	bool bNewPlayerEntered;
};
