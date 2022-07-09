// Fill out your copyright notice in the Description page of Project Settings.


#include "Jetski.h"

// Sets default values
AJetski::AJetski()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!boxCollisionComponent)
	{
		boxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("boxCollisionComponent"));
		boxCollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Trigger"));
		boxCollisionComponent->SetBoxExtent(FVector(160.0f, 85.0f, 50.0f));

		RootComponent = boxCollisionComponent;
	}

	if (!meshComponent)
	{
		meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_Jetski(TEXT("/Game/NonCharacters/SM_Jetski.SM_Jetski"));
		if (SM_Jetski.Succeeded())
		{
			meshComponent->SetStaticMesh(SM_Jetski.Object);
		}
		meshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -31.0f));
		meshComponent->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void AJetski::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJetski::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

