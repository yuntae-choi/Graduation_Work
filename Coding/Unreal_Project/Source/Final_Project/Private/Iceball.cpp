// Fill out your copyright notice in the Description page of Project Settings.


#include "Iceball.h"

// Sets default values
AIceball::AIceball()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	if (!collisionComponent)
	{
		collisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollisionComponent"));
		collisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		collisionComponent->InitSphereRadius(13.0f);
		collisionComponent->BodyInstance.bNotifyRigidBodyCollision = true;
		collisionComponent->OnComponentHit.AddDynamic(this, &AIceball::OnHit);
		collisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		collisionComponent->SetUseCCD(true);

		RootComponent = collisionComponent;
	}

	if (!meshComponent)
	{
		meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SNOWBALL(TEXT("/Game/NonCharacters/iceball1_130.iceball1_130"));
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
		projectileMovementComponent->bSimulationEnabled = false;	// 생성 후 움직이지 않도록 (눈덩이 릴리즈 시 활성화)
	}

	if (!icewallClass)
	{
		static ConstructorHelpers::FClassFinder<AActor> IceWall_Class(TEXT("/Game/Blueprints/IceWallGenerator_BP.IceWallGenerator_BP_C"));
		if (IceWall_Class.Succeeded())
		{
			icewallClass = IceWall_Class.Class;
		}
	}
}

// Called when the game starts or when spawned
void AIceball::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AIceball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIceball::Throw_Implementation(FVector Direction, float Speed)
{
	collisionComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	projectileMovementComponent->bSimulationEnabled = true;	// 아이스볼이 움직일 수 있도록 활성화
	projectileMovementComponent->InitialSpeed = Speed;
	projectileMovementComponent->MaxSpeed = Speed;
	projectileMovementComponent->Velocity = (Direction + FVector(0.0f, 0.0f, 0.15f)) * (projectileMovementComponent->InitialSpeed);

	//Delay 함수
	FTimerHandle WaitHandle;
	float WaitTime = 0.01f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			collisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}), WaitTime, false);
}

void AIceball::IceballThrow_Implementation(FRotator Rotation, float Speed)
{
	collisionComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	projectileMovementComponent->bSimulationEnabled = true;	// 아이스볼이 움직일 수 있도록 활성화
	projectileMovementComponent->InitialSpeed = Speed;
	projectileMovementComponent->MaxSpeed = Speed;
	projectileMovementComponent->Velocity = (Rotation.Vector() + FVector(0.0f, 0.0f, 0.15f)) * (projectileMovementComponent->InitialSpeed);

	//Delay 함수
	FTimerHandle WaitHandle;
	float WaitTime = 0.01f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			collisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}), WaitTime, false);

	rotation = Rotation;
}

// Function that is called when the projectile hits something.
void AIceball::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("loc :%f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z));

	Destroy();

	SpawnIceWall();
}

void AIceball::SpawnIceWall()
{
	if (icewallClass)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			FRotator newRotation = FRotator(0.0f, rotation.Yaw, 0.0f);
			FVector newLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 20.0f);
			FTransform transform = FTransform(newRotation, newLocation, FVector(1.0f));
			World->SpawnActor<AActor>(icewallClass, transform, SpawnParams);
		}
	}
}