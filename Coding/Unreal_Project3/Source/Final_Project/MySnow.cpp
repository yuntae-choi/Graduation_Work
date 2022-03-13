// Fill out your copyright notice in the Description page of Project Settings.


#include "MySnow.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"

// Sets default values
AMySnow::AMySnow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Snow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SNOW"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SNOW(TEXT("/Game/NonCharacters/snowball1.snowball1"));
	if (SM_SNOW.Succeeded())
	{
		Snow->SetStaticMesh(SM_SNOW.Object);
	}

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent")); 
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	RootComponent = CollisionComponent;
	Snow->SetupAttachment(CollisionComponent);

	Snow->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	CollisionComponent->InitSphereRadius(15.0f);

	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent); 
	ProjectileMovementComponent->InitialSpeed = 3000.0f; 
	ProjectileMovementComponent->MaxSpeed = 3000.0f; 
	ProjectileMovementComponent->bRotationFollowsVelocity = true; 
	ProjectileMovementComponent->bShouldBounce = true; 
	ProjectileMovementComponent->Bounciness = 0.3f;

	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AMySnow::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMySnow::FireInDirection(const FVector& ShootDirection)
{ 
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed; 
}