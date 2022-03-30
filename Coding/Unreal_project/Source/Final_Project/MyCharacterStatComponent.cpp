// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterStatComponent.h"
#include "MyGameInstance.h"

// Sets default values for this component's properties
UMyCharacterStatComponent::UMyCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	sessionID = 1;
}


// Called when the game starts
void UMyCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMyCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewSessionID(sessionID);
}


//// Called every frame
//void UMyCharacterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UMyCharacterStatComponent::SetNewSessionID(int32 newSessionID)
{
	auto MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	MYCHECK(nullptr != MyGameInstance);
	CurrentStatData = MyGameInstance->GetMyCharacterData(newSessionID);
	if (nullptr != CurrentStatData)
	{
		sessionID = newSessionID;
		CurrentHP = CurrentStatData->MaxHP;
	}
	else
	{
		MYLOG(Error, TEXT("SessionID (%d) data doesn't exist"), sessionID);
	}
}

void UMyCharacterStatComponent::SetDamage(float NewDamage)
{
	MYCHECK(nullptr != CurrentStatData);
	CurrentHP = FMath::Clamp<float>(CurrentHP = NewDamage, 0.0f, CurrentStatData->MaxHP);
	if (CurrentHP <= 0.0f)
	{
		OnHPIsZero.Broadcast();
	}
}

float UMyCharacterStatComponent::GetAttack()
{
	MYCHECK(nullptr != CurrentStatData, 0.0f);
	return CurrentStatData->Attack;
}