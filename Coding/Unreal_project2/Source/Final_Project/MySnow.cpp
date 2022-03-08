// Fill out your copyright notice in the Description page of Project Settings.


#include "MySnow.h"

// Sets default values
AMySnow::AMySnow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Snow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SNOW"));

	RootComponent = Snow;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SNOW(TEXT("/Game/NonCharacters/snowball1.snowball1"));
	if (SM_SNOW.Succeeded())
	{
		Snow->SetStaticMesh(SM_SNOW.Object);
	}
}

// Called when the game starts or when spawned
void AMySnow::BeginPlay()
{
	Super::BeginPlay();
	
}

