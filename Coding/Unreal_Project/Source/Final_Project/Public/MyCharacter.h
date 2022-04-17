// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "GameFramework/Character.h"
#include "MySnowball.h"
#include "Itembox.h"
#include "Bonfire.h"
#include "MyCharacter.generated.h"

enum CharacterState {
	// 동물,		동물,		눈사람,			눈사람
	AnimalNormal, AnimalSlow, SnowmanNormal, SnowmanStunned
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

	bool CanSetItem();
	void SetItem(class AMyItem* NewItem);
	void SetIsFarming(bool value) { bIsFarming = value; };
	void SetCanFarmItem(AActor* item) { farmingItem = item; };

	bool GetIsInsideOfBonfire() { return bIsInsideOfBonfire; };
	void SetIsInsideOfBonfire(bool value) { bIsInsideOfBonfire = value; };
	void UpdateTemperatureState();
	//void UpdateTemperatureByMatch();
	void UpdateSpeed();
	int GetCharacterState() { return iCharacterState; };
	bool IsSnowman() { return bIsSnowman; };
	void StartStun(float waitTime);
	void EndStun(float waitTime);
	void ResetHasItems();	// 소유한 아이템 및 효과들 초기화
	void Attack();
	

protected:
	virtual void BeginPlay() override;

private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void StartFarming();
	void EndFarming();
	void UpdateFarming(float deltaTime);
	void UpdateHP();
	void ChangeSnowman();
	void WaitForStartGame();
	bool GetItem(int itemType);

public:	

	//UPROPERTY(VisibleAnywhere, Category = Item)
	//class AMyItem* CurrentItem;

	//UPROPERTY(VisibleAnywhere, Category = Item)
	//USkeletalMeshComponent* Item;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* springArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* camera;

	// 현재 손에 들고있는 눈덩이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	AMySnowball* snowball;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iSessionID;

	// 모든 캐릭터 동일 & 변경될 일 x
	static const int iMaxHP;
	static const int iMinHP;
	int		SessionId;		// 플레이어 고유 아이디
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

protected:
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AMySnowball> projectileClass;

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

	//UPROPERTY(VisibleAnywhere, Category = Temperature)
	//float fMatchDuration;	// 후에 성냥 클래스 생성하면 성냥 클래스 변수로 옮기기

	//bool match;

	UPROPERTY(VisibleAnywhere, Category = State)
	int32 iCharacterState;	// 현재 캐릭터의 상태 (AnimalNormal, AnimalSlow, SnowmanNormal, SnowmanStunned)

	bool bIsSnowman;	// 현재 캐릭터가 눈사람인지

	UPROPERTY(VisibleAnywhere, Category = "Data")
	APlayerController* playerController;

	// 스턴 관리하는 타이머 핸들러
	FTimerHandle stunHandle;
};
