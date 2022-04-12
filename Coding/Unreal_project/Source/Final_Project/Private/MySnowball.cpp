// Fill out your copyright notice in the Description page of Project Settings.


#include "MySnowball.h"
#include "MyCharacter.h"

// Sets default values
AMySnowball::AMySnowball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	if (!meshComponent)
	{
		meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SNOWBALL(TEXT("/Game/NonCharacters/snowball1.snowball1"));
		if (SM_SNOWBALL.Succeeded())
		{
			meshComponent->SetStaticMesh(SM_SNOWBALL.Object);
			meshComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
			meshComponent->BodyInstance.bNotifyRigidBodyCollision = true;
			meshComponent->OnComponentHit.AddDynamic(this, &AMySnowball::OnHit);
			meshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			meshComponent->SetSimulatePhysics(false);
			meshComponent->SetUseCCD(true);
			RootComponent = meshComponent;
		}
	}

	iDamage = 10;
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
	meshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	meshComponent->SetSimulatePhysics(true);

	FVector ImpulseVector = FVector(Direction * 15000.0f + FVector(0.0f, 0.0f, 1700.0f));
	meshComponent->AddImpulse(ImpulseVector, NAME_None, true);

	 //Delay ÇÔ¼ö
	FTimerHandle WaitHandle;
	float WaitTime = 0.1f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			meshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}), WaitTime, false);
}

// Function that is called when the projectile hits something.
void AMySnowball::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	MYLOG(Warning, TEXT("collision"));

	auto MyCharacter = Cast<AMyCharacter>(OtherActor);

	if (nullptr != MyCharacter)
	{
		FDamageEvent DamageEvent;
		MyCharacter->TakeDamage(iDamage, DamageEvent, false, this);
	}

	Destroy();
}