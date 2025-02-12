// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "GameFramework/Character.h"
#include "MySnowball.h"
#include "Iceball.h"
#include "Itembox.h"
#include "Bonfire.h"
#include "ClientSocket.h"
#include "SnowballBomb.h"	
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Tornado.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "MyCharacter.generated.h"

enum CharacterState {
	// 동물,		동물,			동물,		눈사람,			눈사람
	AnimalNormal, AnimalSlow, AnimalStunned, SnowmanNormal, SnowmanStunned
};

enum UICategory {
	HP, CurSnowball, CurIceball, CurMatch, MaxSnowIceballAndMatch, HasUmbrella, HasBag, HasShotgun, IsFarmingSnowdrift, SnowdriftFarmDuration, 
	SelectedItem, SelectedProjectile, SelectedWeapon, AllOfUI
};

enum Weapon {
	Hand, Shotgun
};

enum Projectile {
	Snowball, Iceball
};

// 후에 Umbrella 클래스 새로 제작해서 분리해야함 (임시)
enum UmbrellaState {
	UmbClosed, UmbOpening, UmbOpened, UmbClosing
};

UCLASS()
class FINAL_PROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Jump() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = GamePlay)
	void ReleaseSnowball();
	void SendReleaseBullet(int bullet);
	void SendCancelAttack(int bullet);

	void ReleaseIceball();

	void SendSpawnSnowballBomb();


	void SetIsFarming(bool value) { bIsFarming = value; };
	bool GetIsFarming() { return bIsFarming; };
	UFUNCTION(BlueprintCallable, Category = GamePlay)
	void SetCanFarmItem(AActor* item) { farmingItem = item; };
	bool GetItem(int itemType);

	bool GetIsInsideOfBonfire() { return bIsInsideOfBonfire; };
	void SetIsInsideOfBonfire(bool value) { bIsInsideOfBonfire = value; };
	void UpdateTemperatureState();
	void UpdateSpeed();
	int GetCharacterState() { return iCharacterState; };
	bool IsSnowman() { return bIsSnowman; };

	UFUNCTION(BlueprintCallable, Category = GamePlay)
	void StartStun(float waitTime);
	UFUNCTION(BlueprintCallable, Category = GamePlay)
	void EndStun(float waitTime);

	void ResetHasItems();	// 소유한 아이템 및 효과들 초기화
	void Attack();
	void ReleaseAttack();      //호출시 서버로 메세지 전송
	void CancelSnowBallAttack(); //서버에서 전송이 왔을때
	void CancelIceBallAttack(); //서버에서 전송이 왔을때

	void SnowBallAttack();    // 일반 공격
	void ShotgunAttack(); //샷건 공격
	void IceballAttack();    // 아이스볼 공격
	void ShowProjectilePath();
	void HideProjectilePath();

	void ChangeSnowman();
	void ChangeAnimal();	// 캐릭터를 동물화 (부활)
	void SetCharacterMaterial(int Id = 0);	// 캐릭터 색상 설정, 동물->눈사람 머티리얼 변경
	void UpdateUI(int uiCategory, float farmDuration = 0.0f);

	void StartFarming();
	void EndFarming();

	void SelectMatch();
	void SelectUmbrella();
	void UseSelectedItem();
	void ReleaseRightMouseButton();


	UFUNCTION(BlueprintCallable, Category = GamePlay)
	bool GetIsInTornado() { return bIsInTornado; };
	UFUNCTION(BlueprintCallable, Category = GamePlay)
	void SetIsInTornado(bool value) { bIsInTornado = value; };

	void ShowShotgun();
	void HideShotgun();
	void SpawnSnowballBomb();

	void StartUmbrella();
	void ShowUmbrella();
	void HideUmbrella();
	void OpenUmbrellaAnim();
	void CloseUmbrellaAnim();


	int GetUmbrellaState() { return iUmbrellaState; };
	void SetUmbrellaState(int umbrellaState) { iUmbrellaState = umbrellaState; };
	void ReleaseUmbrella();

	void GetBag();

	void GetSupplyBox();

	void GetOnOffJetski();
	void GetOnJetski();
	void GetOffJetski();
	float Getfspeed();

	void Cheat_Teleport1();
	void Cheat_Teleport2();
	void Cheat_Teleport3();
	void Cheat_Teleport4();
	void Cheat_IncreaseHP();
	void Cheat_DecreaseHP();
	void Cheat_IncreaseSnowball();
	void Cheat_SpawnSupplyBox();

	UFUNCTION()
	class UMyAnimInstance* GetAnim() const { return myAnim; }

	void init_Socket();

	bool GetIsSnowman() { return bIsSnowman; };
	bool GetIsAiming() { return bIsAiming; };
	
	void SendFreeze(int iBodyParts);
	void FreezeHead();
	void FreezeLeftForearm();
	void FreezeLeftUpperarm();
	void FreezeRightForearm();
	void FreezeRightUpperarm();
	void FreezeCenter();
	void FreezeLeftThigh();
	void FreezeLeftCalf();
	void FreezeRightThigh();
	void FreezeRightCalf();
	void FreezeAnimation(FTimerHandle& timerHandle, int& frame, bool& end, UStaticMeshComponent*& bone, TArray<UStaticMesh*>& FrozenMeshes);
	void FreezeAnimationEndCheck(FTimerHandle& timerHandle, bool& end);
	void FreezeAnimationEndCheck2(FTimerHandle& timerHandle2, bool& end2);
	void MeltingCheck();
	void MeltingAnimation(FTimerHandle& timerHandle, int& frame, bool& end, UStaticMeshComponent*& bone, TArray<UStaticMesh*>& FrozenMeshes);

	void InitializeFreeze();

	void GetUserId(char id[MAX_NAME_SIZE]) { strcpy_s(id, 20, userId); }; // 아이디 얻기
	void SetUserId(char id[MAX_NAME_SIZE]) { strcpy_s(userId, id); };     // 아이디 세팅

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void UpdateFarming(float deltaTime);
	void UpdateHP();
	
	void WaitForStartGame();	// 게임 시작 후 대기

	void UpdateZByTornado();
	void UpdateControllerRotateByTornado();

	void ChangeWeapon();
	void ChangeProjectile();

	void SetAimingCameraPos();
	void UpdateAiming();

	void UpdateJetski();

	void SettingHead();
	void SettingLeftForearm();
	void SettingLeftUpperArm();
	void SettingRightForearm();
	void SettingRightUpperArm();
	void SettingCenter();
	void SettingLeftThigh();
	void SettingLeftCalf();
	void SettingRightThigh();
	void SettingRightCalf();

public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* springArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* springArm2;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	AActor* aimingCameraPos;

	// jetski 탑승 시 사용될 spring arm, camera
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* springArm3;

	// minimap용
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* springArm4;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* camera3;

	// 현재 손에 들고있는 눈덩이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	AMySnowball* snowball;

	// 현재 손에 들고있는 아이스볼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	AIceball* iceball;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Data", Meta = (AllowPrivateAccess = true))
	int32 iSessionId;

	// 모든 캐릭터 동일 & 변경될 일 x
	static const int iMaxHP;
	static const int iMinHP;
	static const int iBeginSlowHP;	// 캐릭터가 슬로우 상태가 되기 시작하는 hp

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iCurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iMaxSnowballCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iCurrentSnowballCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iMaxIceballCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iCurrentIceballCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iMaxMatchCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iCurrentMatchCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool bHasUmbrella;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool bHasBag = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool bHasShotgun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	bool bIsSnowman;	// 현재 캐릭터가 눈사람인지

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* dynamicMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* bearMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTexture*> bearTextureArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* snowmanMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iSelectedItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool rotateCont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iSelectedWeapon;	// 현재 선택된 무기

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iSelectedProjectile;	// 현재 선택된 발사체

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* shotgunMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> snowballBombDirArray;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	USkeletalMeshComponent* umbrella1MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	USkeletalMeshComponent* umbrella2MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UBoxComponent* umb1CollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UBoxComponent* umb2CollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UNiagaraSystem* smokeNiagara;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* bagMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* jetskiMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool bIsRiding;		// jetski 탑승 중인지

	bool bReleaseUmbrella;
	
	ATornado* overlappedTornado;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	USceneCaptureComponent2D* minimapCaptureComponent;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	class AMyPlayerController* localPlayerController;

	//Freeze Effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	UStaticMeshComponent* headComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	TArray<UStaticMesh*> headMeshes;

	int32 iHeadFrame = 0;
	bool bHeadAnimEnd = false;
	bool bHeadAnimEnd2 = false;
	FTimerHandle headHandle;
	FTimerHandle headHandle2;
	bool bMeltingHead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	UStaticMeshComponent* leftForearmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	TArray<UStaticMesh*> leftForearmMeshes;

	int32 iLeftForearmFrame;
	bool bLeftForearmAnimEnd = false;
	bool bLeftForearmAnimEnd2 = false;
	FTimerHandle leftForearmHandle;
	FTimerHandle leftForearmHandle2;
	bool bMeltingLeftForearm = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	UStaticMeshComponent* leftUpperarmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	TArray<UStaticMesh*> leftUpperarmMeshes;

	int32 iLeftUpperarmFrame;
	bool bLeftUpperarmAnimEnd = false;
	bool bLeftUpperarmAnimEnd2 = false;
	FTimerHandle leftUpperarmHandle;
	FTimerHandle leftUpperarmHandle2;
	bool bMeltingLeftUpperarm = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	UStaticMeshComponent* rightForearmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	TArray<UStaticMesh*> rightForearmMeshes;

	int32 iRightForearmFrame;
	bool bRightForearmAnimEnd = false;
	bool bRightForearmAnimEnd2 = false;
	FTimerHandle rightForearmHandle;
	FTimerHandle rightForearmHandle2;
	bool bMeltingRightForearm = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	UStaticMeshComponent* rightUpperarmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	TArray<UStaticMesh*> rightUpperarmMeshes;

	int32 iRightUpperarmFrame;
	bool bRightUpperarmAnimEnd = false;
	bool bRightUpperarmAnimEnd2 = false;
	FTimerHandle rightUpperarmHandle;
	FTimerHandle rightUpperarmHandle2;
	bool bMeltingRightUpperarm = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	UStaticMeshComponent* centerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	TArray<UStaticMesh*> centerMeshes;

	int32 iCenterFrame;
	bool bCenterAnimEnd = false;
	bool bCenterAnimEnd2 = false;
	FTimerHandle centerHandle;
	FTimerHandle centerHandle2;
	bool bMeltingCenter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	UStaticMeshComponent* leftThighComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	TArray<UStaticMesh*> leftThighMeshes;

	int32 iLeftThighFrame;
	bool bLeftThighAnimEnd = false;
	bool bLeftThighAnimEnd2 = false;
	FTimerHandle leftThighHandle;
	FTimerHandle leftThighHandle2;
	bool bMeltingLeftThigh = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	UStaticMeshComponent* leftCalfComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	TArray<UStaticMesh*> leftCalfMeshes;

	int32 iLeftCalfFrame;
	bool bLeftCalfAnimEnd = false;
	bool bLeftCalfAnimEnd2 = false;
	FTimerHandle leftCalfHandle;
	FTimerHandle leftCalfHandle2;
	bool bMeltingLeftCalf = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	UStaticMeshComponent* rightThighComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	TArray<UStaticMesh*> rightThighMeshes;

	int32 iRightThighFrame;
	bool bRightThighAnimEnd = false;
	bool bRightThighAnimEnd2 = false;
	FTimerHandle rightThighHandle;
	FTimerHandle rightThighHandle2;
	bool bMeltingRightThigh = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	UStaticMeshComponent* rightCalfComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frozen")
	TArray<UStaticMesh*> rightCalfMeshes;

	int32 iRightCalfFrame;
	bool bRightCalfAnimEnd = false;
	bool bRightCalfAnimEnd2 = false;
	FTimerHandle rightCalfHandle;
	FTimerHandle rightCalfHandle2;
	bool bMeltingRightCalf = false;

	int32 iId;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UNiagaraSystem* stunNiagara;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UNiagaraSystem* changeNiagara;

	//UPROPERTY(VisibleDefaultsOnly, Category = Component)
	//UNiagaraComponent* tmpNiagara;

	char userId[MAX_NAME_SIZE]; // 플레이어 아이디
	int  iColor;

	float fElapsedTime = 0.0f;

	USoundBase* HandS; //맨손모드 선택 사운드
	USoundBase* ShotGunS;//삿건모드 선택 사운드

protected:
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AMySnowball> projectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASnowballBomb> shotgunProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AIceball> iceballClass;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool isAttacking;

	UPROPERTY()
	class UMyAnimInstance* myAnim;

	UPROPERTY()
	class USkeletalMesh* bear;

	UPROPERTY()
	class USkeletalMesh* snowman;

	UPROPERTY()
	TSubclassOf<class UAnimInstance> bearAnim;

	UPROPERTY()
	TSubclassOf<class UAnimInstance> snowmanAnim;

	UPROPERTY()
	class UUmbrellaAnimInstance* umb1AnimInst;

	UPROPERTY()
	class UUmbrellaAnimInstance* umb2AnimInst;

	UPROPERTY()
	TSubclassOf<class UAnimInstance> umbrella1Anim;

	UPROPERTY()
	TSubclassOf<class UAnimInstance> umbrella2Anim;

	// 캐릭터가 아이템의 트리거 안에 들어와서 현재 파밍할 수 있는 아이템
	UPROPERTY(VisibleAnywhere, Category = Farm)
	AActor* farmingItem;

	// 현재 파밍중인지
	UPROPERTY(VisibleAnywhere, Category = Farm)
	bool bIsFarming;

	// 현재 모닥불 내부인지 외부인지
	UPROPERTY(VisibleAnywhere, Category = State)
	bool bIsInsideOfBonfire;

	// 초당 체온 증감시키는 타이머 핸들러
	FTimerHandle temperatureHandle;

	UPROPERTY(VisibleAnywhere, Category = State)
	int32 iCharacterState;	// 현재 캐릭터의 상태 (AnimalNormal, AnimalSlow, SnowmanNormal, SnowmanStunned)

	UPROPERTY(VisibleAnywhere, Category = "Data")
	APlayerController* playerController;

	// 스턴 관리하는 타이머 핸들러
	FTimerHandle stunHandle;

	bool bIsInTornado;

	int32 iUmbrellaState;

	USplineComponent* projectilePath;
	TArray<USplineMeshComponent*> splineMeshComponents;
	USceneComponent* projectilePathStartPos;

	bool bIsAiming;
	float fAimingElapsedTime;

	UPROPERTY(VisibleAnywhere, Category = Jetski)
	TSubclassOf<class AJetski> jetskiClass;

	UAnimationAsset* driveAnimAsset;
};
