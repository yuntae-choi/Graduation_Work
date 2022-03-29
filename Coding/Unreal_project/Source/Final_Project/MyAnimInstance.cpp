// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"

UMyAnimInstance::UMyAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Animations/Bear/bear_Skeleton_Montage.bear_Skeleton_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = FVector(Pawn->GetVelocity() * FVector(1.0f, 1.0f, 0.0f)).Size();
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			float MoveForward = Character->GetInputAxisValue(TEXT("UpDown"));
			float MoveRight = Character->GetInputAxisValue(TEXT("LeftRight"));
			CurrentPawnDirection = UKismetMathLibrary::DegAtan2(MoveForward, MoveRight);
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

void UMyAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.0f);
}
