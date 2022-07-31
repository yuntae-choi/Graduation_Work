// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HoloLens/AllowWindowsPlatformAtomics.h"
#include <thread>
#include <atomic>
//#include <queue>
#include "HoloLens/HideWindowsPlatformAtomics.h"

#include "Final_Project.h"
#include "MyCharacter.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundCue.h"
#include "GameFramework/PlayerController.h"
#include "ClientSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_HP, int32, NewHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_CurrentSnowballCount, int32, NewCurrentSnowballCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_CurrentIceballCount, int32, NewCurrentIceballCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_CurrentMatchCount, int32, NewCurrentMatchCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDele_Dynamic_MaxSnowIceballAndMatchCount, int32, NewMaxSnowballCount, int32, NewMaxIceballCount, int32, NewMaxMatchCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_HasUmbrella, bool, NewHasUmbrella);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_HasBag, bool, NewHasBag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_HasShotgun, bool, NewHasShotgun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_IsFarmingSnowdrift, bool, NewIsFarmingSnowdrift);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_SnowdriftFarmDuration, float, NewSnowdriftFarmDuration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_SelectedItem, int32, NewSelectedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_SelectedProjectile, int32, NewSelectedProjectile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_SelectedWeapon, int32, NewSelectedWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDele_Dynamic_KillLog, FString, iAttacker, FString, iVictim, int32, Cause, int32, iKillLogType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_GameResult, int32, iWinnerId);


/**
 *
 */	
UCLASS()
class FINAL_PROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMyPlayerController();

	cCharactersInfo* GetCharactersInfo() const { return charactersInfo; }
	ClientSocket* GetSocket() const { return mySocket; }
	void SetSessionId(const int sessionId) { iSessionId = sessionId; }
	void SetCharactersInfo(cCharactersInfo* ci_) { if (ci_ != nullptr)	charactersInfo = ci_; }
	void SetNewCharacterInfo(shared_ptr<cCharacter> NewPlayer_);
	void SetNewTornadoInfo(shared_ptr<cCharacter> newTornado);

	void SetInitInfo(const cCharacter& me);

	void SetDestroySnowdritt(const int s_id);
	void SetDestroyIcedritt(const int obj_id);
	void SetDestroyitembox(const int obj_id);
	void SetDestroySpBox(const int obj_id);
	void SetDestroyPlayer(const int del_sid);
	void SetGameEnd(const int target_id);
	void SetOpenItembox(const int obj_id);
	void SetAttack(const int s_id, int at_type);
	void SetFreeze(const int s_id, int body_type);
	void StartFreeze(AMyCharacter* player_, cCharacter* info);
	void SetItem(const int s_id, int item_type, bool end);
	void SetSocket();
	void GetItem(int sId, int itemType);


	void UpdateTornado();


	void SendPlayerInfo(int input);
	void SendTeleportInfo(int input);
	void SendCheatInfo(int input);

	UFUNCTION(BlueprintCallable)
	void PlayerReady();	// 컨트롤러를 소유한 플레이어가 레디하면 호출되는 함수
	UFUNCTION(BlueprintCallable)
	void PlayerUnready();
	void StartGame();	// 모든 플레이어가 ready하면 호출 (ReadyUI 제거, 게임에 대한 입력 허용)
	void LoadGameResultUI(int winnerSessionId);	// 게임 종료 시 결과창 ui 띄우기 (CharacterUI 제거, 승자 id 인자로 받아서 결과 출력)
	
	UFUNCTION(BlueprintCallable)
	void BtnCreateAccount(FString id, FString pw);
	UFUNCTION(BlueprintCallable)
	void BtnLogin(FString id, FString pw);

	void DeleteLoginUICreateReadyUI();

	UFUNCTION(BlueprintCallable)
	FString GetUserId(int sessionId);

	// UpdateUI 델리게이트 이벤트 호출
	void CallDelegateUpdateAllOfUI();
	void CallDelegateUpdateHP();
	void CallDelegateUpdateCurrentSnowballCount();
	void CallDelegateUpdateCurrentIceballCount();
	void CallDelegateUpdateCurrentMatchCount();
	void CallDelegateUpdateMaxSnowIceballAndMatchCount();
	void CallDelegateUpdateHasUmbrella();
	void CallDelegateUpdateHasBag();
	void CallDelegateUpdateHasShotgun();
	void CallDelegateUpdateIsFarmingSnowdrift();
	void CallDelegateUpdateSnowdriftFarmDuration(float farmDuration);
	void CallDelegateUpdateSelectedItem();
	void CallDelegateUpdateSelectedProjectile();
	void CallDelegateUpdateSelectedWeapon();
	void CallDelegateUpdateKillLog(int attacker, int victim, int cause);
	void CallDelegateUpdateGameResult(int winnerId);

	void SpawnSupplyBox(float x, float y, float z = 4500.0f);	// 해당 위치에 보급상자 스폰


	void SetCharacterState(const int s_id, STATE_Type _state)
	{
		charactersInfo->players[s_id].myState = _state;
	}
	void SetCharacterHP(const int s_id, int _hp)
	{
		charactersInfo->players[s_id].HealthValue = _hp;
	}
	void StartSignal()
	{
		//bInGame = true;
		StartGame();
	}
	bool IsStart()
	{
		return bInGame;
	}
	void GetBone()
	{
		bIsBone = true;
	}
	void SetCnt(int iBear, int iSnowman)
	{
		iBearCnt = iBear;
	    iSnowmanCnt = iSnowman;
	}
	UFUNCTION(BlueprintCallable)
	int GetBearCnt()
	{
		return iBearCnt;
	}
	UFUNCTION(BlueprintCallable)
	int GetSnowmanCnt()
	{
		return iSnowmanCnt;
	}
protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* pawn_) override;

private:
	void InitPlayerSetting();   
	bool UpdateWorldInfo();		// 월드 동기화
	void UpdateNewPlayer();     // 타 플레이어 생성
	void UpdateNewTornado();    // 토네이도 생성
	void UpdatePlayerInfo(cCharacter& info); // 플레이어 정보 업데이트
	void UpdatePlayerLocation();
	void LoadReadyUI();	// ReadyUI 띄우기, UI에 대한 입력만 허용
	void LoadCharacterUI();	// CharacterUI 띄우기, 게임에 대한 입력 허용
	void LoadLoginUI();	// LoginUI 띄우기, UI에 대한 입력만 허용
	void Reset_Items(int s_id);
	void FixRotation();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	int							iSessionId;			// 캐릭터의 세션 고유 아이디
	bool                        bTornado;       // 토네이도 접속 여부       


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool bIsReady;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> readyUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class UUserWidget* readyUI;	// 레디 UI (레디 버튼 및 텍스트)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> characterUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class UUserWidget* characterUI;	// 캐릭터 UI (체력, )

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> gameResultUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class UUserWidget* gameResultUI;	// 게임 결과 UI (게임종료 후 승패 ui)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> loginUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class UUserWidget* loginUI;	// 로그인 UI (계정 생성 버튼, 로그인 버튼, id pw 입력)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FString loginInfoText;	// 로그인 UI info text에 보여질 text

	// 델리게이트 이벤트 UpdateUI
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_HP FuncUpdateHP;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_CurrentSnowballCount FuncUpdateCurrentSnowballCount;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_CurrentIceballCount FuncUpdateCurrentIceballCount;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_CurrentMatchCount FuncUpdateCurrentMatchCount;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_MaxSnowIceballAndMatchCount FuncUpdateMaxSnowIceballAndMatchCount;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_HasUmbrella FuncUpdateHasUmbrella;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_HasBag FuncUpdateHasBag;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_HasShotgun FuncUpdateHasShotgun;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_IsFarmingSnowdrift FuncUpdateIsFarmingSnowdrift;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_SnowdriftFarmDuration FuncUpdateSnowdriftFarmDuration;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_SelectedItem FuncUpdateSelectedItem;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_SelectedProjectile FuncUpdateSelectedProjectile;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_SelectedWeapon FuncUpdateSelectedWeapon;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_KillLog FuncUpdateKillLog;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_GameResult FuncUpdateGameResult;

	ClientSocket* mySocket;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	bool	bInGame = false;

	UAudioComponent* AudioComponent;
	USoundCue* LoginCue;
	USoundCue* ReadyCue;
	USoundCue* InGameCue;
	USoundBase* ThrowS;
	USoundBase* ShootS;
	USoundBase* WalkS;
	USoundBase* ClickS;
	USoundBase* FreezeS;


private:
	
	cCharacter				initInfo;
	LockQueue<shared_ptr<cCharacter>>				newPlayers;			// 플레이어 로그인 시 캐릭터 정보
	LockQueue<shared_ptr<cCharacter>>				newTornados;			// 토네이도 로그인 시 캐릭터 정보

		//새 플레이어 스폰
	shared_ptr<cCharacter> newplayer;
	shared_ptr<cCharacter> newtornado;

	//queue<shared_ptr<cCharacter>>				newPlayers;			// 플레이어 로그인 시 캐릭터 정보
	atomic<int> ivictoryPlayer;


	//queue <int>				newBalls;
	cCharactersInfo*		charactersInfo;	// 다른 캐릭터들의 정보

	bool							bNewPlayerEntered;
	bool							bInitPlayerSetting;
	bool							bIsBone = false;
	int                             iBearCnt;
	int                             iSnowmanCnt;


	atomic<bool>                    bSetStart;
	// 스폰시킬 다른 캐릭터
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ACharacter> WhoToSpawn;
	TSubclassOf<class APawn> TornadoToSpawn;


	//bool							bIsConnected;	// 서버와 접속 유무

	AMyCharacter* localPlayerCharacter;

	float fOldYaw = 0.0f;
	float bIsSpeedZero = false;

	UPROPERTY(VisibleAnywhere, Category = Class)
	TSubclassOf<class AActor> supplyboxClass;

 


};
