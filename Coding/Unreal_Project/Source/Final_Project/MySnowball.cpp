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

	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("SnowballPreset"));
		CollisionComponent->InitSphereRadius(10.0f);
		// ECC_GameTraceChannel1 (MyCharacter) - 생성 시 캐릭터에 대한 콜리전 ignore (release 될 때 block으로 변경) (자기 자신과의 충돌 방지용)
		CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
		CollisionComponent->SetSimulatePhysics(false);
		CollisionComponent->SetUseCCD(true);	// 연속 충돌 검사 (눈덩이가 빠른 이동으로 콜리전 무시하는 오류 방지)

		RootComponent = CollisionComponent;
	}

	if (!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SnowballMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Game/NonCharacters/snowball1.snowball1"));
		if (Mesh.Succeeded())
		{
			MeshComponent->SetStaticMesh(Mesh.Object);
			//MeshComponent->SetRelativeScale3D(FVector(3.0f));
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

// Called every frame
void AMySnowball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMySnowball::Throw_Implementation(FVector Direction)
{
	FVector ImpulseVector = FVector((Direction * 15000.0f) + FVector(0.0f, 0.0f, 1200.0f));
	
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->AddImpulse(ImpulseVector);
	// release 시 캐릭터에 대한 콜리전 block으로 변경
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

	// Delay 함수
	//FTimerHandle WaitHandle;
	//float WaitTime = 0.05f;
	//GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	//	{
	//		// release 0.05 후 캐릭터에 대한 콜리전 block으로 변경
	//		CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	//	}), WaitTime, false);
}