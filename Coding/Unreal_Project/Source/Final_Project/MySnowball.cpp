// Fill out your copyright notice in the Description page of Project Settings.


#include "MySnowball.h"

// Sets default values
AMySnowball::AMySnowball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//if (!RootComponent)
	//{
	//	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	//}

	if (!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SnowballMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Game/NonCharacters/snowball1.snowball1"));
		if (Mesh.Succeeded())
		{
			MeshComponent->BodyInstance.SetCollisionProfileName(TEXT("SnowballPreset"));
			MeshComponent->SetStaticMesh(Mesh.Object);
			//MeshComponent->SetRelativeScale3D(FVector(3.0f));
			//MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// ECC_GameTraceChannel1 (MyCharacter) - 생성 시 캐릭터에 대한 콜리전 ignore (release 될 때 block으로 변경)
			MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
			MeshComponent->SetSimulatePhysics(false);
			MeshComponent->SetUseCCD(true);
		}

		static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("/Game/Materials/Mat_Original_Snow.Mat_Original_Snow"));
		if (Material.Succeeded())
		{
			MaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, MeshComponent);
		}
		MeshComponent->SetMaterial(0, MaterialInstance);
		RootComponent = MeshComponent;
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
	//MeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	MeshComponent->SetSimulatePhysics(true);
	FVector ImpulseVector = FVector(Direction * 5000.0f + FVector(0.0f, 0.0f, 200.0f));
	MeshComponent->AddImpulse(ImpulseVector, NAME_None, true);

	// Delay 함수
	FTimerHandle WaitHandle;
	float WaitTime = 0.05f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			//MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			// release 0.05 후 캐릭터에 대한 콜리전 block으로 변경
			MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
		}), WaitTime, false);
}