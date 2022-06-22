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

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK2_MONTAGE(TEXT("/Game/Animations/Bear/BearThrow2Montage.BearThrow2Montage"));
	if (ATTACK2_MONTAGE.Succeeded())
	{
		attack2Montage = ATTACK2_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_SHOTGUN_MONTAGE(TEXT("/Game/Animations/Bear/BearShotgunMontage.BearShotgunMontage"));
	if (ATTACK_SHOTGUN_MONTAGE.Succeeded())
	{
		attackShotgunMontage = ATTACK_SHOTGUN_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> UMBRELLA_MONTAGE(TEXT("/Game/Animations/Bear/BearUmbrellaMontage.BearUmbrellaMontage"));
	if (UMBRELLA_MONTAGE.Succeeded())
	{
		umbrellaMontage = UMBRELLA_MONTAGE.Object;
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

void UMyAnimInstance::PlayAttack2Montage()
{
	if (!bIsDead)
		Montage_Play(attack2Montage, 1.0f);
}

void UMyAnimInstance::PlayAttack2MontageSectionEnd()
{
	if (!bIsDead)
	{
		Montage_Play(attack2Montage, 1.0f);
		Montage_JumpToSection(FName("End"));
		bThrowing = false;

		auto Pawn = TryGetPawnOwner();
		if (!::IsValid(Pawn)) return;
		auto MyCharacter = Cast<AMyCharacter>(Pawn);
		if (nullptr == MyCharacter) return;
		MyCharacter->HideProjectilePath();
	}
}

void UMyAnimInstance::AnimNotify_SnowballRelease2()
{
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	auto MyCharacter = Cast<AMyCharacter>(Pawn);
	if (nullptr == MyCharacter) return;

	MyCharacter->SendReleaseSnowball();
}

void UMyAnimInstance::AnimNotify_StopThrow()
{
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	auto MyCharacter = Cast<AMyCharacter>(Pawn);
	if (nullptr == MyCharacter) return;

	bThrowing = true;
	MyCharacter->ShowProjectilePath();
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

void UMyAnimInstance::PlayUmbrellaMontage()
{
	if (!bIsDead)
		Montage_Play(umbrellaMontage, 1.0f);
}

void UMyAnimInstance::ResumeUmbrellaMontage()
{
	if (!bIsDead)
		Montage_Resume(umbrellaMontage);
}

void UMyAnimInstance::AnimNotify_SpawnUmbrella()
{
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	auto MyCharacter = Cast<AMyCharacter>(Pawn);
	if (nullptr == MyCharacter) return;

	MyCharacter->ShowUmbrella();
}

void UMyAnimInstance::AnimNotify_DestroyUmbrella()
{
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	auto MyCharacter = Cast<AMyCharacter>(Pawn);
	if (nullptr == MyCharacter) return;

	MyCharacter->HideUmbrella();
}

void UMyAnimInstance::AnimNotify_FullyOpenedUmbrella()
{
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	auto MyCharacter = Cast<AMyCharacter>(Pawn);
	if (nullptr == MyCharacter) return;

	if (MyCharacter->bReleaseUmbrella)
	{
		if (MyCharacter->iSessionId == MyCharacter->localPlayerController->iSessionId)
		{
			MYLOG(Warning, TEXT("ReleaseUmbrella"));
			MyCharacter->localPlayerController->SendPlayerInfo(COMMAND_UMB_END);
		}
		//MyCharacter->CloseUmbrellaAnim();
		return;
	}
	
	Montage_Pause(umbrellaMontage);
	MyCharacter->SetUmbrellaState(UmbrellaState::UmbOpened);
}