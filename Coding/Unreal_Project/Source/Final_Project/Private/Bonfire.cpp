// Fill out your copyright notice in the Description page of Project Settings.


#include "Bonfire.h"

// Sets default values
ABonfire::ABonfire()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!sphereComponent)
	{
		sphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComponent"));
		sphereComponent->BodyInstance.SetCollisionProfileName(TEXT("Trigger"));
		sphereComponent->SetSphereRadius(500.0f);
		sphereComponent->SetHiddenInGame(false);	// 디버깅용 (모닥불 범위 보이도록)

		RootComponent = sphereComponent;
	}

	sphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABonfire::OnComponentBeginOverlap);
	sphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABonfire::OnComponentEndOverlap);
}

// Called when the game starts or when spawned
void ABonfire::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABonfire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABonfire::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* mycharacter = Cast<AMyCharacter>(OtherActor);
	if (mycharacter)
	{
		mycharacter->SetIsInsideOfBonfire(true);
		mycharacter->UpdateTemperatureState();
	}
}

void ABonfire::OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCharacter* mycharacter = Cast<AMyCharacter>(OtherActor);
	if (mycharacter)
	{
		mycharacter->SetIsInsideOfBonfire(false);
		mycharacter->UpdateTemperatureState();
	}
}