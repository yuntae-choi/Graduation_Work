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
	int _my_session_id;
	int _my_x;
	int _my_y;
	int _my_z;
	int _other_session_id;
	int _other_x;
	int _other_y;
	int _other_z;

	// 스폰시킬 다른 캐릭터
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ACharacter> WhoToSpawn;

	// 새 플레이어 업데이트
	void RecvNewPlayer(int sessionID, float x, float y, float z);

	void UpdateNewPlayer();		// 플레이어 동기화
	void UpdatePlayerInfo(int input);
	void UpdatePlayerS_id(int _s_id);

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool bNewPlayerEntered;
};
