// Fill out your copyright notice in the Description page of Project Settings.


#include "MyItemBox.h"
#include "MyItem.h"
#include "MyCharacter.h"

// Sets default values
AMyItemBox::AMyItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));

	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(27.0f, 42.0f, 15.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/NonCharacters/itembox_down.itembox_down"));

	if (SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Object);
	}

	//static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));

	//if (P_CHESTOPEN.Succeeded())
	//{
	//	Effect->SetTemplate(P_CHESTOPEN.Object);
	//	Effect->bAutoActivate = false;
	//}

	Box->SetRelativeLocation(FVector(0.0f, 0.0f, -15.0f));

	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));

	ItemClass = AMyItem::StaticClass();
}

// Called when the game starts or when spawned
void AMyItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AMyItemBox::OnCharacterOverlap);
}

// Called every frame
void AMyItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto MyCharacter = Cast<AMyCharacter>(OtherActor);
	MYCHECK(nullptr != MyCharacter);

	if (nullptr != MyCharacter && nullptr != ItemClass)
	{
		MYLOG(Warning, TEXT("%s get item."), *MyCharacter->GetName());


		//if (MyCharacter->CanSetItem())
		//{
		//	auto NewItem = GetWorld()->SpawnActor<AMyItem>(ItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
		//	MyCharacter->SetItem(NewItem);
		//	//Effect->Activate(true);
		//	Box->SetHiddenInGame(true, true);
		//	SetActorEnableCollision(false);
		//	//Effect->OnSystemFinished.AddDynamic(this, &AMyItemBox::OnEffectFinished);
		//	Destroy();
		//}
		//else
		//{
		//	MYLOG(Warning, TEXT("%s can't get item currently."), *MyCharacter->GetName());
		//}
	}
}

//void AMyItemBox::OnEffectFinished(UParticleSystemComponent* PSystem)
//{
//	Destroy();
//}
