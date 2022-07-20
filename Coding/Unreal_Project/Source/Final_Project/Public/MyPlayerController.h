// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HoloLens/AllowWindowsPlatformAtomics.h"
#include <thread>
#include <atomic>
//#include <queue>
#include "HoloLens/HideWindowsPlatformAtomics.h"

#include "Final_Project.h"
#include "MyCharacter.h"
#include "GameFramework/PlayerController.h"
#include "ClientSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_HP, int32, NewHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_CurrentSnowballCount, int32, NewCurrentSnowballCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_CurrentIceballCount, int32, NewCurrentIceballCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_CurrentMatchCount, int32, NewCurrentMatchCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDele_Dynamic_MaxSnowIceballAndMatchCount, int32, NewMaxSnowballCount, int32, NewMaxIceballCount, int32, NewMaxMatchCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_HasUmbrella, bool, NewHasUmbrella);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_HasBag, bool, NewHasBag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_IsFarmingSnowdrift, bool, NewIsFarmingSnowdrift);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_SnowdriftFarmDuration, float, NewSnowdriftFarmDuration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_SelectedItem, int32, NewSelectedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_SelectedProjectile, int32, NewSelectedProjectile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_GameResult, bool, GameResult);

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
	void SetInitInfo(const cCharacter& me);
	void SetNewBall(const int s_id);
	void SetRelAttack(const int s_id);
	void SetGunFire(const int s_id);
	void SetJetSki(const int s_id);

	void SetDestroySnowdritt(const int s_id);
	void SetDestroyitembox(const int obj_id);
	void SetDestroyPlayer(const int del_sid);
	void SetGameEnd(const int target_id);
	void SetOpenItembox(const int obj_id);
	void SetAttack(const int s_id);
	void SetShotGun(const int s_id);
	void SetUmb(const int s_id, bool end);
	void SetSocket();
	void get_item(int itemType);

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

	// UpdateUI 델리게이트 이벤트 호출
	void CallDelegateUpdateAllOfUI();
	void CallDelegateUpdateHP();
	void CallDelegateUpdateCurrentSnowballCount();
	void CallDelegateUpdateCurrentIceballCount();
	void CallDelegateUpdateCurrentMatchCount();
	void CallDelegateUpdateMaxSnowIceballAndMatchCount();
	void CallDelegateUpdateHasUmbrella();
	void CallDelegateUpdateHasBag();
	void CallDelegateUpdateIsFarmingSnowdrift();
	void CallDelegateUpdateSnowdriftFarmDuration(float farmDuration);
	void CallDelegateUpdateSelectedItem();
	void CallDelegateUpdateSelectedProjectile();
	void CallDelegateUpdateGameResult(bool isWinner);

	void SetCharacterState(const int s_id, STATE_Type _state)
	{
		charactersInfo->players[s_id].My_State = _state;
	}
	void SetCharacterHP(const int s_id, int _hp)
	{
		charactersInfo->players[s_id].HealthValue = _hp;
	}
	void Start_Signal()
	{
		//bInGame = true;
		StartGame();
	}
	bool is_start()
	{
		return bInGame;
	}
	void get_bone()
	{
		bisBone = true;
	}
protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* pawn_) override;

private:
	void InitPlayerSetting();
	bool UpdateWorldInfo();		// 월드 동기화
	void UpdateNewPlayer();
	void UpdatePlayerInfo(cCharacter& info);
	void LoadReadyUI();	// ReadyUI 띄우기, UI에 대한 입력만 허용
	void LoadCharacterUI();	// CharacterUI 띄우기, 게임에 대한 입력 허용
	void LoadLoginUI();	// LoginUI 띄우기, UI에 대한 입력만 허용
	void Reset_Items(int s_id);
	void FixRotation();

public:
	int							iSessionId;			// 캐릭터의 세션 고유 아이디
	int							itonardoId = -1;			// 토네이도 아이디
	//int							itonardoId;			// 토네이도 아이디


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
	FDele_Dynamic_IsFarmingSnowdrift FuncUpdateIsFarmingSnowdrift;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_SnowdriftFarmDuration FuncUpdateSnowdriftFarmDuration;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_SelectedItem FuncUpdateSelectedItem;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_SelectedProjectile FuncUpdateSelectedProjectile;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_GameResult FuncUpdateGameResult;

	ClientSocket* mySocket;
	bool	bInGame = false;
private:
	
	cCharacter				initInfo;
	LockQueue<shared_ptr<cCharacter>>				newPlayers;			// 플레이어 로그인 시 캐릭터 정보
		//새 플레이어 스폰
	shared_ptr<cCharacter> newplayer;
	//queue<shared_ptr<cCharacter>>				newPlayers;			// 플레이어 로그인 시 캐릭터 정보
	LockQueue<int> newBalls;
	LockQueue<int> destory_snowdrift;
	LockQueue<int> destory_itembox;
	LockQueue<int> open_itembox;
	LockQueue<int> destory_player;
	atomic<int> victory_player;


	//queue <int>				newBalls;
	cCharactersInfo*		charactersInfo;	// 다른 캐릭터들의 정보

	bool							bNewPlayerEntered;
	bool							bInitPlayerSetting;
	bool							bisBone = false;

	atomic<bool>                    bSetStart;
	// 스폰시킬 다른 캐릭터
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ACharacter> WhoToSpawn;
	TSubclassOf<class APawn> TornadoToSpawn;


	//bool							bIsConnected;	// 서버와 접속 유무

	AMyCharacter* localPlayerCharacter;

	float fOldYaw = 0.0f;
	float bIsSpeedZero = false;
};
