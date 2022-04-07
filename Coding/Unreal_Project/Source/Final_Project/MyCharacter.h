// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MySnowball.h"
#include "MyCharacter.generated.h"

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

	bool CanSetItem();
	void SetItem(class AMyItem* NewItem);
	void SetDamage(float newDamage);
	void SetIsFarming(bool value) { bIsFarming = value; };
	void SetCanFarmItem(AActor* item) { farmingItem = item; };

protected:
	virtual void BeginPlay() override;

private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

	void Attack();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable, Category = GamePlay)
	void ReleaseSnowball();

	void StartFarming();
	void EndFarming();
	void UpdateFarming(float deltaTime);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float fMaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float fCurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float fAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iMaxSnowballCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iCurrentSnowballCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iPlusMaxSnowballCountByABag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool bHasUmbrella;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool bHasBag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iMaxMatchCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 iCurrentMatchCount;

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
};
