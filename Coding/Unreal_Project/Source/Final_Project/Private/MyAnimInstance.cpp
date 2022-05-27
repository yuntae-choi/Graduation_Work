// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "MyCharacter.h"
#include "Debug.h"

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

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_SHOTGUN_MONTAGE(TEXT("/Game/Animations/Bear/BearShotgunMontage.BearShotgunMontage"));
	if (ATTACK_SHOTGUN_MONTAGE.Succeeded())
	{
		attackShotgunMontage = ATTACK_SHOTGUN_MONTAGE.Object;
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
		auto Character = Cast<AMyCharacter>(Pawn);
		if (Character)
		{
			float MoveForward = Character->GetInputAxisValue(TEXT("UpDown"));
			float MoveRight = Character->GetInputAxisValue(TEXT("LeftRight"));
#ifdef MULTIPLAY_DEBUG
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
			if (PlayerController && Character->iSessionId == PlayerController->iSessionId)
				fCurrentPawnDirection = UKismetMathLibrary::DegAtan2(MoveForward, MoveRight);
#endif
#ifdef SINGLEPLAY_DEBUG
			fCurrentPawnDirection = UKismetMathLibrary::DegAtan2(MoveForward, MoveRight);
#endif
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

	MyCharacter->SendReleaseSnowball();
}

void UMyAnimInstance::PlayAttackShotgunMontage()
{
	if (!bIsDead)
		Montage_Play(attackShotgunMontage, 1.0f);
}

void UMyAnimInstance::AnimNotify_SpawnShotgun()
{
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	auto MyCharacter = Cast<AMyCharacter>(Pawn);
	if (nullptr == MyCharacter) return;

	MyCharacter->ShowShotgun();
}

void UMyAnimInstance::AnimNotify_DestroyShotgun()
{
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	auto MyCharacter = Cast<AMyCharacter>(Pawn);
	if (nullptr == MyCharacter) return;

	MyCharacter->HideShotgun();
}

void UMyAnimInstance::AnimNotify_SpawnSnowballBomb()
{
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	auto MyCharacter = Cast<AMyCharacter>(Pawn);
	if (nullptr == MyCharacter) return;
	MyCharacter->SendSpawnSnowballBomb(); //샷건 발사 전송
	//MyCharacter->SpawnSnowballBomb();
}