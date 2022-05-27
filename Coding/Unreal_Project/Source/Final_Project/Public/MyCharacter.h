// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "GameFramework/Character.h"
#include "MySnowball.h"
#include "Itembox.h"
#include "Bonfire.h"
#include "ClientSocket.h"
#include "SnowballBomb.h"
#include "MyCharacter.generated.h"

enum CharacterState {
	// 동물,		동물,			동물,		눈사람,			눈사람
	AnimalNormal, AnimalSlow, AnimalStunned, SnowmanNormal, SnowmanStunned
};

enum UICategory {
	HP, CurSnowball, CurMatch, MaxSnowballAndMatch, HasUmbrella, HasBag, IsFarmingSnowdrift, SnowdriftFarmDuration, SelectedItem, AllOfUI
};

enum Weapon {
	Hand, Shotgun
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

	void SetIsFarming(bool value) { bIsFarming = value; };
	bool GetIsFarming() { return bIsFarming; };
	void SetCanFarmItem(AActor* item) { farmingItem = item; };

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
	void SnowAttack();
	void ChangeSnowman();
	void ChangeAnimal();	// 캐릭터를 동물화 (부활)
	void SetCharacterMaterial(int Id = 0);	// 캐릭터 색상 설정, 동물->눈사람 머티리얼 변경
	void UpdateUI(int uiCategory, float farmDuration=0.0f);

	void StartFarming();
	void EndFarming();

	void SelectMatch();
	void SelectUmbrella();
	void UseSelectedItem();


	UFUNCTION(BlueprintCallable, Category = GamePlay)
	bool GetIsInTornado() { return bIsInTornado; };
	UFUNCTION(BlueprintCallable, Category = GamePlay)
	void SetIsInTornado(bool value) { bIsInTornado = value; };

	void ShowShotgun();
	void HideShotgun();
	void SpawnSnowballBomb();

	UFUNCTION()
	class UMyAnimInstance* GetAnim() const { return myAnim; }

	void init_Socket();
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
	bool GetItem(int itemType);
	bool GetIsSnowman() { return bIsSnowman; };
	void WaitForStartGame();	// 게임 시작 후 대기

	void UpdateZByTornado();
	void UpdateControllerRotateByTornado();

	void AttackShotgun();
	void ChangeWeapon();

public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* springArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* camera;

	// 현재 손에 들고있는 눈덩이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	AMySnowball* snowball;

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

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* shotgunMeshComponent;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AMySnowball> projectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASnowballBomb> shotgunProjectileClass;

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

	UPROPERTY(VisibleAnywhere, Category = "Data")
	class AMyPlayerController* localPlayerController;

	// 스턴 관리하는 타이머 핸들러
	FTimerHandle stunHandle;

	bool bIsInTornado;
};
