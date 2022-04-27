// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "MySnowball.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"

//#define CHECKTRAJECTORY	// 눈덩이가 던져지는 시점에서부터 충돌할 때까지의 궤적 로그 출력

// Sets default values
AMySnowball::AMySnowball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

#ifdef CHECKTRAJECTORY
	// 틱 활성화 (궤적 로그)
	PrimaryActorTick.bCanEverTick = true;
	bCheckTrajectory = false;
#endif

	if (!collisionComponent)
	{
		collisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollisionComponent"));
		collisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		collisionComponent->InitSphereRadius(10.0f);
		collisionComponent->BodyInstance.bNotifyRigidBodyCollision = true;
		collisionComponent->OnComponentHit.AddDynamic(this, &AMySnowball::OnHit);
		collisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		projectileMovementComponent->InitialSpeed = 4000.0f;	// 눈덩이 속도 설정
		projectileMovementComponent->MaxSpeed = 4000.0f;
		projectileMovementComponent->bSimulationEnabled = false;	// 생성 후 움직이지 않도록 (눈덩이 릴리즈 시 활성화)
	}

	iDamage = 10;
}

// Called when the game starts or when spawned
void AMySnowball::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMySnowball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#ifdef CHECKTRAJECTORY
	if (bCheckTrajectory) UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
#endif
}

void AMySnowball::Throw_Implementation(FVector Direction)
{
	collisionComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	projectileMovementComponent->bSimulationEnabled = true;	// 눈덩이가 움직일 수 있도록 활성화
	projectileMovementComponent->Velocity = (Direction + FVector(0.0f, 0.0f, 0.1f)) * (projectileMovementComponent->InitialSpeed);

	//Delay 함수
	FTimerHandle WaitHandle;
	float WaitTime = 0.01f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			collisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}), WaitTime, false);

#ifdef CHECKTRAJECTORY
	bCheckTrajectory = true;
#endif
}

// Function that is called when the projectile hits something.
void AMySnowball::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	auto MyCharacter = Cast<ACharacter>(OtherActor);

	if (nullptr != MyCharacter)
	{
		FDamageEvent DamageEvent;
		MyCharacter->TakeDamage(iDamage, DamageEvent, false, this);
	}
	else
		MYLOG(Warning, TEXT("no damage"));

	Destroy();
	
#ifdef CHECKTRAJECTORY
	bCheckTrajectory = false;
#endif
}