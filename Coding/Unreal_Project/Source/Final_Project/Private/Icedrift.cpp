// Fill out your copyright notice in the Description page of Project Settings.


#include "Icedrift.h"
#include "MyCharacter.h"

const float AIcedrift::fFarmDurationMax = 3.0f;
const int AIcedrift::iNumOfIceball = 5;

// Sets default values
AIcedrift::AIcedrift()
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
		meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IcedriftMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Game/NonCharacters/icedrift2.icedrift2"));
		if (Mesh.Succeeded())
		{
			meshComponent->SetStaticMesh(Mesh.Object);
		}
		meshComponent->SetupAttachment(RootComponent);
	}

	boxComponent->OnComponentBeginOverlap.AddDynamic(this, &AIcedrift::OnComponentBeginOverlap);
	boxComponent->OnComponentEndOverlap.AddDynamic(this, &AIcedrift::OnComponentEndOverlap);

	fFarmDuration = fFarmDurationMax;
}

// Called when the game starts or when spawned
void AIcedrift::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIcedrift::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIcedrift::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* mycharacter = Cast<AMyCharacter>(OtherActor);
	if (mycharacter)
	{
		mycharacter->SetCanFarmItem(this);
	}
}

void AIcedrift::OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCharacter* mycharacter = Cast<AMyCharacter>(OtherActor);
	if (mycharacter)
	{
		mycharacter->SetCanFarmItem(nullptr);
		mycharacter->SetIsFarming(false);
		mycharacter->UpdateUI(UICategory::IsFarmingSnowdrift);
	}
}