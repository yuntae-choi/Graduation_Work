// Fill out your copyright notice in the Description page of Project Settings.


#include "MyItemBox.h"
#include "MyItem.h"
#include "MyCharacter.h"

// Sets default values
AMyItemBox::AMyItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));
	effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));

	RootComponent = trigger;
	box->SetupAttachment(RootComponent);
	effect->SetupAttachment(RootComponent);

	trigger->SetBoxExtent(FVector(27.0f, 42.0f, 15.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/NonCharacters/itembox_down.itembox_down"));

	if (SM_BOX.Succeeded())
	{
		box->SetStaticMesh(SM_BOX.Object);
	}

	//static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));

	//if (P_CHESTOPEN.Succeeded())
	//{
	//	effect->SetTemplate(P_CHESTOPEN.Object);
	//	effect->bAutoActivate = false;
	//}

	box->SetRelativeLocation(FVector(0.0f, 0.0f, -15.0f));

	trigger->SetCollisionProfileName(TEXT("ItemBox"));
	box->SetCollisionProfileName(TEXT("NoCollision"));

	itemClass = AMyItem::StaticClass();
}

// Called when the game starts or when spawned
void AMyItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	trigger->OnComponentBeginOverlap.AddDynamic(this, &AMyItemBox::OnCharacterOverlap);
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

	if (nullptr != MyCharacter && nullptr != itemClass)
	{
		MYLOG(Warning, TEXT("%s get item."), *MyCharacter->GetName());


		//if (MyCharacter->CanSetItem())
		//{
		//	auto NewItem = GetWorld()->SpawnActor<AMyItem>(itemClass, FVector::ZeroVector, FRotator::ZeroRotator);
		//	MyCharacter->SetItem(NewItem);
		//	//effect->Activate(true);
		//	box->SetHiddenInGame(true, true);
		//	SetActorEnableCollision(false);
		//	//effect->OnSystemFinished.AddDynamic(this, &AMyItemBox::OnEffectFinished);
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
