// Fill out your copyright notice in the Description page of Project Settings.


#include "MySnowball.h"
#include "MyCharacter.h"

// Sets default values
AMySnowball::AMySnowball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//// Use a sphere as a simple collision representation.
	//CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	//// Set the sphere's collision profile name to "Projectile".
	//CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	//CollisionComponent->BodyInstance.bNotifyRigidBodyCollision = true;
	//// Event called when component hits something.
	//CollisionComponent->OnComponentHit.AddDynamic(this, &AMySnowball::OnHit);
	//// Set the sphere's collision radius.
	//CollisionComponent->InitSphereRadius(10.0f);

	//CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//CollisionComponent->SetSimulatePhysics(false);
	//// Set the root component to be the collision component.
	//RootComponent = CollisionComponent;

	if (!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SNOWBALL(TEXT("/Game/NonCharacters/snowball1.snowball1"));
		if (SM_SNOWBALL.Succeeded())
		{
			MeshComponent->SetStaticMesh(SM_SNOWBALL.Object);
			MeshComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
			MeshComponent->BodyInstance.bNotifyRigidBodyCollision = true;
			MeshComponent->OnComponentHit.AddDynamic(this, &AMySnowball::OnHit);
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			MeshComponent->SetSimulatePhysics(false);
			RootComponent = MeshComponent;
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
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetSimulatePhysics(true);
	FVector ImpulseVector = FVector(Direction * 2000.0f + FVector(0.0f, 0.0f, 200.0f));
	MeshComponent->AddImpulse(ImpulseVector, NAME_None, true);

	// Delay ÇÔ¼ö
	//FTimerHandle WaitHandle;
	//float WaitTime = 0.1f;
	//GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	//	{
	//		
	//	}), WaitTime, false);
}

// Function that is called when the projectile hits something.
void AMySnowball::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	MYLOG(Warning, TEXT("collision"));

	auto MyCharacter = Cast<AMyCharacter>(OtherActor);

	if (nullptr != MyCharacter)
	{
		FDamageEvent DamageEvent;
		MyCharacter->TakeDamage(fAttack, DamageEvent, false, this);
	}

	Destroy();
}