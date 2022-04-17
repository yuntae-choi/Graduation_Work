// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "MyCharacter.h"

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

void UMyAnimInstance::AnimNotify_SnowballRelease()
{
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	auto MyCharacter = Cast<AMyCharacter>(Pawn);
	if (nullptr == MyCharacter) return;
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	
	MyCharacter->ReleaseSnowball(PlayerController->CharactersInfo->players[MyCharacter->iSessionID].FMyLocation,
	PlayerController->CharactersInfo->players[MyCharacter->iSessionID].FMyDirection);
}

void UMyAnimInstance::Anim_SnowballRelease()
{
	//auto Pawn = TryGetPawnOwner();
	//if (!::IsValid(Pawn)) return;

	//auto MyCharacter = Cast<AMyCharacter>(Pawn);
	//if (nullptr == MyCharacter) return;

	//MyCharacter->ReleaseSnowball();
}