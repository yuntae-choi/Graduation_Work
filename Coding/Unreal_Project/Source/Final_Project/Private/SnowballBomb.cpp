// Fill out your copyright notice in the Description page of Project Settings.


#include "SnowballBomb.h"
#include "MyCharacter.h"

// Sets default values
ASnowballBomb::ASnowballBomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!collisionComponent)
	{
		collisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollisionComponent"));
		collisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		collisionComponent->InitSphereRadius(10.0f);
		collisionComponent->BodyInstance.bNotifyRigidBodyCollision = true;
		collisionComponent->OnComponentHit.AddDynamic(this, &ASnowballBomb::OnHit);
		collisionComponent->SetUseCCD(true);

		RootComponent = collisionComponent;
	}

	if (!meshComponent)
	{
		meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SNOWBALL(TEXT("/Game/NonCharacters/snowball1.snowball1"));
		if (SM_SNOWBALL.Succeeded())
		{
			meshComponent->SetStaticMesh(SM_SNOWBALL.Object);
			meshComponent->BodyInstance.SetCollisionProfileName(TEXT("NoCollision"));
		}
	}

	meshComponent->SetupAttachment(RootComponent);

	if (!projectileMovementComponent)
	{
		projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		projectileMovementComponent->SetUpdatedComponent(collisionComponent);
		projectileMovementComponent->InitialSpeed = 2500.0f;	// 눈덩이 속도 설정
		projectileMovementComponent->MaxSpeed = 2500.0f;
	}

	iDamage = 10;
}

// Called when the game starts or when spawned
void ASnowballBomb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASnowballBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASnowballBomb::Throw_Implementation(FVector Direction)
{
	projectileMovementComponent->Velocity = (Direction + FVector(0.0f, 0.0f, 0.15f)) * (projectileMovementComponent->InitialSpeed);
}

// Function that is called when the projectile hits something.
void ASnowballBomb::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	auto MyCharacter = Cast<ACharacter>(OtherActor);

	if (nullptr != MyCharacter)
	{
		// 우산과 충돌했는지 확인
		UBoxComponent* boxCollision = Cast<UBoxComponent>(OtherComponent);
		if (boxCollision)
		{
			//UE_LOG(LogTemp, Warning, TEXT("no damage, hit umbrella"));
		}
		else
		{
			FDamageEvent DamageEvent;
			MyCharacter->TakeDamage(iDamage, DamageEvent, false, this);
		}
	}
	else
	{
		//MYLOG(Warning, TEXT("no damage"));
	}

	if (Cast<ASnowballBomb>(OtherActor)) return;	// snowball bomb끼리 충돌 시 무시

	Destroy();
}