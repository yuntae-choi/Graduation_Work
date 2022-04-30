// Fill out your copyright notice in the Description page of Project Settings.


#include "Snowdrift.h"
#include "MyCharacter.h"

const float ASnowdrift::fFarmDurationMax = 3.0f;
const int ASnowdrift::iNumOfSnowball = 10;
int ASnowdrift::iIdCountHelper = -1;

// Sets default values
ASnowdrift::ASnowdrift()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!boxComponent)
	{
		boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComponent"));
		boxComponent->BodyInstance.SetCollisionProfileName(TEXT("Trigger"));
		boxComponent->SetBoxExtent(FVector(50.0f, 50.0f, 16.0f));

		RootComponent = boxComponent;
	}

	if (!meshComponent)
	{
		meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SnowdriftMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Game/NonCharacters/snowdrift1.snowdrift1"));
		if (Mesh.Succeeded())
		{
			meshComponent->SetStaticMesh(Mesh.Object);
		}
		meshComponent->SetupAttachment(RootComponent);
	}

	boxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASnowdrift::OnComponentBeginOverlap);
	boxComponent->OnComponentEndOverlap.AddDynamic(this, &ASnowdrift::OnComponentEndOverlap);

	fFarmDuration = fFarmDurationMax;
}

// Called when the game starts or when spawned
void ASnowdrift::BeginPlay()
{
	Super::BeginPlay();

	iId = ++iIdCountHelper;
}

// Called every frame
void ASnowdrift::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASnowdrift::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* mycharacter = Cast<AMyCharacter>(OtherActor);
	if (mycharacter)
	{
		mycharacter->SetCanFarmItem(this);
	}
}

void ASnowdrift::OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCharacter* mycharacter = Cast<AMyCharacter>(OtherActor);
	if (mycharacter)
	{
		mycharacter->SetCanFarmItem(nullptr);
		mycharacter->SetIsFarming(false);
	}
}