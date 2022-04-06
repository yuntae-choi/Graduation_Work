// Fill out your copyright notice in the Description page of Project Settings.


#include "MyItem.h"

// Sets default values
AMyItem::AMyItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Umbrella = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UMBRELLA"));
	//RootComponent = Umbrella;

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_UMBRELLA(TEXT("/Game/NonCharacters/Items/SM_Umbrella.SM_Umbrella"));
	//if (SM_UMBRELLA.Succeeded())
	//{
	//	Umbrella->SetStaticMesh(SM_UMBRELLA.Object);
	//}

	//Umbrella->SetCollisionProfileName(TEXT("NoCollision"));

}

// Called when the game starts or when spawned
void AMyItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

