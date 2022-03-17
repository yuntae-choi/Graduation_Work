// Fill out your copyright notice in the Description page of Project Settings.


#include "MySnow.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"

// Sets default values
AMySnow::AMySnow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (!CollisionComponent)
	{
		// Use a sphere as a simple collision representation.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set the sphere's collision profile name to "Projectile".
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		// Event called when component hits something.
		CollisionComponent->OnComponentHit.AddDynamic(this, &AMySnow::OnHit);
		// Set the sphere's collision radius.
		CollisionComponent->InitSphereRadius(15.0f);
		// Set the root component to be the collision component.
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		//ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if (!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Game/NonCharacters/snowball1.snowball1"));
		if (Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}

		static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("/Game/Materials/Mat_Original_Snow.Mat_Original_Snow"));
		if (Material.Succeeded())
		{
			ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
		}
		ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
		ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
		ProjectileMeshComponent->SetupAttachment(RootComponent);
	}


	// Delete the projectile after 3 seconds.
	InitialLifeSpan = 3.0f;

	//Snow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SNOW"));

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SNOW(TEXT("/Game/NonCharacters/snowball1.snowball1"));
	//if (SM_SNOW.Succeeded())
	//{
	//	Snow->SetStaticMesh(SM_SNOW.Object);
	//}

	//CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent")); 
	//ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	//RootComponent = CollisionComponent;
	//Snow->SetupAttachment(CollisionComponent);

	//Snow->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	//CollisionComponent->InitSphereRadius(15.0f);

	//ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent); 
	//ProjectileMovementComponent->InitialSpeed = 3000.0f; 
	//ProjectileMovementComponent->MaxSpeed = 3000.0f; 
	//ProjectileMovementComponent->bRotationFollowsVelocity = true; 
	//ProjectileMovementComponent->bShouldBounce = true; 
	//ProjectileMovementComponent->Bounciness = 0.3f;

	//InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AMySnow::BeginPlay()
{
	Super::BeginPlay();
	
}

// Function that is called when the projectile hits something.
void AMySnow::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 10.0f, Hit.ImpactPoint);
	}

	//Destroy();
}

void AMySnow::FireInDirection(const FVector& ShootDirection)
{ 
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed; 
}