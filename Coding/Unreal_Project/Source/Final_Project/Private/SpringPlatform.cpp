// Fill out your copyright notice in the Description page of Project Settings.


#include "SpringPlatform.h"
#include "MyCharacter.h"

const float fSpringScaleZInterval = 0.25f;
const float fPlatformLocationZInterval = 12.25f;

// Sets default values
ASpringPlatform::ASpringPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!sceneComponent)
	{
		sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

		RootComponent = sceneComponent;
	}

	if (!springMeshComponent)
	{
		springMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpringMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>SpringMesh(TEXT("/Game/NonCharacters/JumpSpring_SM.JumpSpring_SM"));
		if (SpringMesh.Succeeded())
		{
			springMeshComponent->SetStaticMesh(SpringMesh.Object);
		}
		springMeshComponent->SetupAttachment(RootComponent);
	}

	if (!platformMeshComponent)
	{
		platformMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>PlatformMesh(TEXT("/Game/NonCharacters/JumpPlatform_SM.JumpPlatform_SM"));
		if (PlatformMesh.Succeeded())
		{
			platformMeshComponent->SetStaticMesh(PlatformMesh.Object);
		}
		platformMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 74.0f));
		platformMeshComponent->SetupAttachment(RootComponent);
	}

	if (!boxComponent)
	{
		boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComponent"));
		boxComponent->SetBoxExtent(FVector(65.0f, 65.0f, 12.0f));
		boxComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 25.0f));

		boxComponent->SetupAttachment(platformMeshComponent);
	}

	boxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASpringPlatform::OnComponentBeginOverlap);

	iSpringPlatformState = SpringPlatformState::Normal;
	fSpringScaleZ = 1.0f;
	fPlatformLocationZ = 74.0f;
}

// Called when the game starts or when spawned
void ASpringPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpringPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpring();
}

void ASpringPlatform::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* mycharacter = Cast<AMyCharacter>(OtherActor);
	if (mycharacter)
	{
		iSpringPlatformState = SpringPlatformState::Increasing;	// 스프링이 늘어나도록

		mycharacter->GetCharacterMovement()->AddImpulse(FVector(0.0f, 0.0f, 2500.0f), true);	// 캐릭터가 튀어오르도록

		//FTimerHandle WaitHandle;
		//float WaitTime = 0.2f;
		//GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		//	{
		//		iSpringPlatformState = SpringPlatformState::Increasing;	// 스프링이 늘어나도록
		//	}), WaitTime, false);
	}
}

void ASpringPlatform::UpdateSpring()
{
	switch (iSpringPlatformState)
	{
	case SpringPlatformState::Normal:
		break;
	case SpringPlatformState::Maximum:
		break;
	case SpringPlatformState::Increasing:
		if (fSpringScaleZ < 3.0f)
		{	// 스프링 늘리기
			fSpringScaleZ += fSpringScaleZInterval;
			springMeshComponent->SetWorldScale3D(FVector(1.0f, 1.0f, fSpringScaleZ));
			fPlatformLocationZ += fPlatformLocationZInterval;
			platformMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, fPlatformLocationZ));
		}
		else
		{
			iSpringPlatformState = SpringPlatformState::Maximum;

			// 스프링이 최대크기에 도달하면 x초 후 크기가 줄어들도록
			FTimerHandle WaitHandle;
			float WaitTime = 2.0f;
			GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
				{
					iSpringPlatformState = SpringPlatformState::Decreasing;
				}), WaitTime, false);
		}
		break;
	case SpringPlatformState::Decreasing:
		if (fSpringScaleZ > 1.0f)
		{	// 스프링 줄이기
			fSpringScaleZ -= fSpringScaleZInterval;
			springMeshComponent->SetWorldScale3D(FVector(1.0f, 1.0f, fSpringScaleZ));
			fPlatformLocationZ -= fPlatformLocationZInterval;
			platformMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, fPlatformLocationZ));
		}
		else
		{
			iSpringPlatformState = SpringPlatformState::Normal;
		}
		break;
	default:
		break;
	}
}