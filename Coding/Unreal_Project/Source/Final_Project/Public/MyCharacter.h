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
#include "MyCharacter.generated.h"

enum CharacterState {
	// 동물,		동물,			동물,		눈사람,			눈사람
	AnimalNormal, AnimalSlow, AnimalStunned, SnowmanNormal, SnowmanStunned
};

enum UICategory {
	HP, CurSnowball, CurIceball, CurMatch, MaxSnowIceballAndMatch, HasUmbrella, HasBag, IsFarmingSnowdrift, SnowdriftFarmDuration, 
	SelectedItem, SelectedProjectile, AllOfUI
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

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = GamePlay)
	void ReleaseSnowball();
	void SendReleaseSnowball();
	void SendReleaseAttack();

	void ReleaseIceball();

	void SendSpawnSnowballBomb();


	void SetIsFarming(bool value) { bIsFarming = value; };
	bool GetIsFarming() { return bIsFarming; };
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
	void Recv_ReleaseAttack(); //서버에서 전송이 왔을때

	void SnowAttack();    // 일반 공격
	void AttackShotgun(); //샷건 공격
	void IceballAttack();    // 아이스볼 공격
	void ShowProjectilePath();
	void HideProjectilePath();

	void ChangeSnowman();
	void ChangeAnimal();	// 캐릭터를 동물화 (부활)
	void SetCharacterMaterial(int Id = 0);	// 캐릭터 색상 설정, 동물->눈사람 머티리얼 변경
	void UpdateUI(int uiCategory, float farmDuration=0.0f);

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

	void GetOnOffJetski();
	void GetOnJetski();
	void GetOffJetski();

	void Cheat_Teleport1();
	void Cheat_Teleport2();
	void Cheat_Teleport3();
	void Cheat_Teleport4();
	void Cheat_IncreaseHP();
	void Cheat_DecreaseHP();
	void Cheat_IncreaseSnowball();

	UFUNCTION()
	class UMyAnimInstance* GetAnim() const { return myAnim; }

	void init_Socket();

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
	void FreezeAnimation(TArray<UStaticMeshComponent*> bones, int& frame, bool& end);
	void FreezeAnimationEndCheck(FTimerHandle& timerHandle, bool& end);
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
	
	bool GetIsSnowman() { return bIsSnowman; };
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

	bool	IsAlive;		// 살아있는지
	bool	IsAttacking;	// 공격중인지


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
	bool bHasBag;

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

	UPROPERTY(VisibleAnywhere, Category = "Data")
	class AMyPlayerController* localPlayerController;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head1;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head2;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head3;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head4;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head5;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head6;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head7;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head8;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head9;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head10;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head11;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head12;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head13;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head14;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head15;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head16;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head17;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head18;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head19;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head20;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head21;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head22;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* head23;

	TArray<UStaticMeshComponent*> heads;
	int32 iHeadFrame;
	bool bHeadAnimEnd = false;
	FTimerHandle headHandle;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm1;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm2;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm3;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm4;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm5;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm6;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm7;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm8;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm9;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm10;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm11;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm12;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm13;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm14;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm15;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm16;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm17;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm18;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm19;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm20;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm21;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm22;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftForearm23;

	TArray<UStaticMeshComponent*> leftForearms;
	int32 iLeftForearmFrame;
	bool bLeftForearmAnimEnd = false;
	FTimerHandle leftForearmHandle;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm1;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm2;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm3;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm4;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm5;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm6;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm7;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm8;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm9;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm10;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm11;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm12;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm13;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm14;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm15;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm16;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm17;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm18;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm19;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm20;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm21;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm22;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftUpperarm23;

	TArray<UStaticMeshComponent*> leftUpperarms;
	int32 iLeftUpperarmFrame;
	bool bLeftUpperarmAnimEnd = false;
	FTimerHandle leftUpperarmHandle;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm1;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm2;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm3;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm4;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm5;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm6;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm7;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm8;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm9;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm10;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm11;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm12;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm13;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm14;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm15;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm16;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm17;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm18;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm19;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm20;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm21;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm22;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightForearm23;

	TArray<UStaticMeshComponent*> rightForearms;
	int32 iRightForearmFrame;
	bool bRightForearmAnimEnd = false;
	FTimerHandle rightForearmHandle;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm1;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm2;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm3;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm4;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm5;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm6;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm7;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm8;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm9;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm10;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm11;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm12;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm13;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm14;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm15;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm16;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm17;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm18;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm19;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm20;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm21;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm22;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightUpperarm23;

	TArray<UStaticMeshComponent*> rightUpperarms;
	int32 iRightUpperarmFrame;
	bool bRightUpperarmAnimEnd = false;
	FTimerHandle rightUpperarmHandle;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center1;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center2;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center3;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center4;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center5;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center6;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center7;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center8;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center9;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center10;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center11;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center12;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center13;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center14;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center15;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center16;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center17;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center18;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center19;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center20;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center21;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center22;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* center23;

	TArray<UStaticMeshComponent*> centers;
	int32 iCenterFrame;
	bool bCenterAnimEnd = false;
	FTimerHandle centerHandle;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh1;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh2;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh3;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh4;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh5;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh6;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh7;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh8;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh9;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh10;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh11;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh12;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh13;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh14;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh15;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh16;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh17;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh18;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh19;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh20;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh21;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh22;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftThigh23;

	TArray<UStaticMeshComponent*> leftThighs;
	int32 iLeftThighFrame;
	bool bLeftThighAnimEnd = false;
	FTimerHandle leftThighHandle;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf1;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf2;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf3;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf4;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf5;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf6;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf7;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf8;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf9;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf10;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf11;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf12;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf13;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf14;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf15;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf16;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf17;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf18;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf19;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf20;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf21;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf22;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* leftCalf23;

	TArray<UStaticMeshComponent*> leftCalfs;
	int32 iLeftCalfFrame;
	bool bLeftCalfAnimEnd = false;
	FTimerHandle leftCalfHandle;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh1;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh2;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh3;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh4;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh5;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh6;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh7;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh8;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh9;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh10;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh11;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh12;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh13;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh14;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh15;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh16;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh17;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh18;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh19;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh20;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh21;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh22;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightThigh23;

	TArray<UStaticMeshComponent*> rightThighs;
	int32 iRightThighFrame;
	bool bRightThighAnimEnd = false;
	FTimerHandle rightThighHandle;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf1;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf2;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf3;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf4;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf5;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf6;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf7;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf8;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf9;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf10;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf11;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf12;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf13;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf14;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf15;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf16;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf17;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf18;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf19;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf20;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf21;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf22;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
		UStaticMeshComponent* rightCalf23;

	TArray<UStaticMeshComponent*> rightCalfs;
	int32 iRightCalfFrame;
	bool bRightCalfAnimEnd = false;
	FTimerHandle rightCalfHandle;

	int32 iId;

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
