// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"

UMyAnimInstance::UMyAnimInstance()
{
	fCurrentPawnSpeed = 0.0f;
	bIsInAir = false;
	bIsDead = false;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Animations/Bear/BearThrowMontage.BearThrowMontage"));

	if (ATTACK_MONTAGE.Succeeded())
	{
		attackMontage = ATTACK_MONTAGE.Object;
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	if (!bIsDead)
	{
		fCurrentPawnSpeed = FVector(Pawn->GetVelocity() * FVector(1.0f, 1.0f, 0.0f)).Size();
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			float MoveForward = Character->GetInputAxisValue(TEXT("UpDown"));
			float MoveRight = Character->GetInputAxisValue(TEXT("LeftRight"));
			fCurrentPawnDirection = UKismetMathLibrary::DegAtan2(MoveForward, MoveRight);
			bIsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

void UMyAnimInstance::PlayAttackMontage()
{
	if (!bIsDead)
		Montage_Play(attackMontage, 1.0f);
}
