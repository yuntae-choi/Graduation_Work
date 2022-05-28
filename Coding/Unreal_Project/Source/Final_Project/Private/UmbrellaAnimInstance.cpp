// Fill out your copyright notice in the Description page of Project Settings.


#include "UmbrellaAnimInstance.h"

UUmbrellaAnimInstance::UUmbrellaAnimInstance()
{
	bOpen = false;
	bClose = false;
}

void UUmbrellaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;


}