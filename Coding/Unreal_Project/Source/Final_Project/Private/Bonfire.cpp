// Fill out your copyright notice in the Description page of Project Settings.


#include "Bonfire.h"
#include "MyCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

const int ABonfire::iHealAmount = 10;
const int ABonfire::iDamageAmount = 1;

// Sets default values
ABonfire::ABonfire()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!sphereComponent)
	{
		sphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComponent"));
		sphereComponent->BodyInstance.SetCollisionProfileName(TEXT("Trigger"));
		sphereComponent->SetSphereRadius(500.0f);
		sphereComponent->SetHiddenInGame(false);	// 디버깅용 (모닥불 범위 보이도록)

		RootComponent = sphereComponent;
	}

	sphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABonfire::OnComponentBeginOverlap);
	sphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABonfire::OnComponentEndOverlap);
	sphereComponent->OnComponentActivated.AddDynamic(this, &ABonfire::OnComponentActivated);
}

// Called when the game starts or when spawned
void ABonfire::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateOverlapCharacters();	// 처음 시작할 때는 begin overlap 이벤트가 발생하지 않아서 이렇게 처리해주어야 한다
}

// Called every frame
void ABonfire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABonfire::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* mycharacter = Cast<AMyCharacter>(OtherActor);
	if (mycharacter)
	{	// begin overlap 하는 캐릭터의 체력 증가하도록 변경
		mycharacter->SetIsInsideOfBonfire(true);
		mycharacter->UpdateTemperatureState();
	}
}

void ABonfire::OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	// end overlap 하는 캐릭터의 체력 감소하도록 변경
	AMyCharacter* mycharacter = Cast<AMyCharacter>(OtherActor);
	if (mycharacter)
	{
		mycharacter->SetIsInsideOfBonfire(false);
		mycharacter->UpdateTemperatureState();
	}
}

void ABonfire::UpdateOverlapCharacters()
{
	TArray<AActor*> overlapActorsArray;	// overlap 중인 캐릭터들을 담을 배열
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // overlap 가능한 오브젝트 타입 - MyCharacter
	TEnumAsByte<EObjectTypeQuery> MyCharacter = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1); // ECC_GameTraceChannel1 - MyCharacter
	ObjectTypes.Add(MyCharacter);
	TArray<AActor*> emptyArray;

	// overlap 중인 캐릭터들 배열에 담기
	UKismetSystemLibrary::ComponentOverlapActors(sphereComponent, GetActorTransform(), ObjectTypes, nullptr, emptyArray, overlapActorsArray);

	for (AActor* actor : overlapActorsArray)
	{	// overlap 중인 캐릭터들의 체력 증가하도록 변경
		AMyCharacter* character = Cast<AMyCharacter>(actor);
		if (character)
		{
			character->SetIsInsideOfBonfire(true);
			character->UpdateTemperatureState();
		}
	}
}