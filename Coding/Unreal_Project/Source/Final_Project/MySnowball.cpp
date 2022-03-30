// Fill out your copyright notice in the Description page of Project Settings.


#include "MySnowball.h"

// Sets default values
AMySnowball::AMySnowball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	//if (!CollisionComponent)
	//{
	//	// Use a sphere as a simple collision representation.
	//	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	//	// Set the sphere's collision profile name to "Projectile".
	//	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	//	// Set the sphere's collision radius.
	//	CollisionComponent->InitSphereRadius(10.0f);
	//	//CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//	//CollisionComponent->SetSimulatePhysics(false);
	//	// Set the root component to be the collision component.
	//	RootComponent = CollisionComponent;
	//}

	if (!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Game/NonCharacters/snowball1.snowball1"));
		if (Mesh.Succeeded())
		{
			MeshComponent->SetStaticMesh(Mesh.Object);
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			MeshComponent->SetSimulatePhysics(false);
		}

		static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("/Game/Materials/Mat_Original_Snow.Mat_Original_Snow"));
		if (Material.Succeeded())
		{
			MaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, MeshComponent);
		}
		MeshComponent->SetMaterial(0, MaterialInstance);
		MeshComponent->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void AMySnowball::BeginPlay()
{
	Super::BeginPlay();
	
}

//// Called every frame
//void AMySnowball::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AMySnowball::Throw_Implementation(FVector Direction)
{
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	MeshComponent->SetSimulatePhysics(true);
	FVector ImpulseVector = FVector(Direction * 5000.0f + FVector(0.0f, 0.0f, 200.0f));
	MeshComponent->AddImpulse(ImpulseVector, NAME_None, true);

	// Delay ÇÔ¼ö
	FTimerHandle WaitHandle;
	float WaitTime = 0.1f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}), WaitTime, false);
}