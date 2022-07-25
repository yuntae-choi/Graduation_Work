// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyPlayerController.h"
#include "MyAnimInstance.generated.h"


/**
 * 
 */
UCLASS()
class FINAL_PROJECT_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UMyAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void PlayAttack2Montage();
	void PlayAttack2MontageSectionEnd();
	void PlayAttackShotgunMontage();
	void PlayUmbrellaMontage();
	void ResumeUmbrellaMontage();
	void SetDead() { bIsDead = true; }

	float GetDirection() const { return fCurrentPawnDirection; }
	void SetDirection(const float dir) { fCurrentPawnDirection = dir; }

	bool GetIsInAir() { return bIsInAir; }

private:
	UFUNCTION()
	void AnimNotify_SnowballRelease();
	UFUNCTION()
	void AnimNotify_SnowballRelease2();
	UFUNCTION()
	void AnimNotify_StopThrow();

	UFUNCTION()
	void AnimNotify_SpawnShotgun();
	UFUNCTION()
	void AnimNotify_DestroyShotgun();
	UFUNCTION()
	void AnimNotify_SpawnSnowballBomb();

	UFUNCTION()
	void AnimNotify_SpawnUmbrella();
	UFUNCTION()
	void AnimNotify_DestroyUmbrella();
	UFUNCTION()
	void AnimNotify_FullyOpenedUmbrella();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Throw, Meta = (AllowPrivateAccess = true))
	bool bThrowing;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float fCurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float fCurrentPawnDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool bIsInAir;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* attackMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* attack2Montage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* attackShotgunMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* umbrellaMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool bIsDead;
};
