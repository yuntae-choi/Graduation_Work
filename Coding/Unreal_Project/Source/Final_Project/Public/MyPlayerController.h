// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <thread>
#include <queue>
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

	void SetSessionId(const int sessionId) { iSessionId = sessionId; }
	void SetCharactersInfo(cCharactersInfo* ci_) { if (ci_ != nullptr)	CharactersInfo = ci_; }
	void SetNewCharacterInfo(shared_ptr<cCharacter> NewPlayer_);
	void SetInitInfo(const cCharacter& me);

	//void RecvNewPlayer(const cCharacter& info);
	//void RecvNewBall(int s_id);

	////void UpdateNewPlayer(int new_s_id, float new_x, float new_y, float new_z);
	void UpdatePlayerInfo(int input);
	//void UpdateFarming(int item_no);
	//void UpdatePlayerInfo(const cCharacter& info);		// 플레이어 동기화	
	//void Throw_Snow(FVector MyLocation, FVector MyDirection);
	//
	//void UpdatePlayerS_id(int _s_id);

	// 스폰시킬 다른 캐릭터
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ACharacter> WhoToSpawn;

	//void UpdateRotation();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void InitPlayerSetting();
	bool UpdateWorldInfo();		// 월드 동기화
	void UpdateNewPlayer();

public:
	int							iSessionId;			// 캐릭터의 세션 고유 아이디
private:
	ClientSocket*			mySocket;
	cCharacter				initInfo;
	queue<shared_ptr<cCharacter>>				NewCharactersInfo;			// 플레이어 로그인 시 캐릭터 정보
	cCharactersInfo*		CharactersInfo;	// 다른 캐릭터들의 정보
	int							iPlayerCount;

	FTimerHandle			SendPlayerInfoHandle;	// 동기화 타이머 핸들러

	bool							bNewPlayerEntered;
	bool							bInitPlayerSetting;
	

	//bool							bIsConnected;	// 서버와 접속 유무
	//void UpdateNewBall();

	//void StartPlayerInfo(const cCharacter& info);
	//
	//int32 iMySessionId;
	//float fMy_x;
	//float fMy_y;
	//float fMy_z;
	//int32 iOtherSessionId;
	//float fOther_x;
	//float fOther_y;
	//float fOther_z;
	//// 새 플레이어 입장
	//queue <int> iNewPlayers;
	//queue <int> iNewBalls;

	//int	nPlayers;

};
