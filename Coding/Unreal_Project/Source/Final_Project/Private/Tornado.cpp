// Fill out your copyright notice in the Description page of Project Settings.

#include "Tornado.h"
#include "MyCharacter.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATornado::ATornado()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!collisionComponent)
	{
		collisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollisionComponent"));
		collisionComponent->SetCapsuleHalfHeight(800.0f);
		collisionComponent->SetCapsuleRadius(250.0f);
		//collisionComponent->SetHiddenInGame(false);	// 디버깅용 (토네이도 범위 보이도록)

		RootComponent = collisionComponent;
	}

	collisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATornado::OnComponentBeginOverlap);
	collisionComponent->OnComponentEndOverlap.AddDynamic(this, &ATornado::OnComponentEndOverlap);

	//이펙트
	tornadoNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_TORNADO(TEXT("/Game/FX/NS_Tornado.NS_Tornado"));
	if (NS_TORNADO.Succeeded())
	{
		tornadoNiagara->SetAsset(NS_TORNADO.Object);
		tornadoNiagara->SetRelativeLocation(FVector(0.0f, 0.0f, -1200.0f));
		tornadoNiagara->SetupAttachment(collisionComponent);
	}
}

// Called when the game starts or when spawned
void ATornado::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATornado::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATornado::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATornado::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* mycharacter = Cast<AMyCharacter>(OtherActor);
	if (mycharacter)
	{
		mycharacter->SetIsInTornado(true);
		mycharacter->StartStun(3.5f);
		mycharacter->overlappedTornado = this;
	}
}

void ATornado::OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCharacter* mycharacter = Cast<AMyCharacter>(OtherActor);
	if (mycharacter)
	{
		mycharacter->SetIsInTornado(false);
		mycharacter->overlappedTornado = nullptr;

		float randomFloat;
		bool randomBool;

		randomFloat = UKismetMathLibrary::RandomFloatInRange(600.0f, 1000.0f);
		randomBool = UKismetMathLibrary::RandomBool();
		float impulseX = UKismetMathLibrary::SelectFloat(randomFloat, -randomFloat, randomBool);

		randomFloat = UKismetMathLibrary::RandomFloatInRange(600.0f, 1000.0f);
		randomBool = UKismetMathLibrary::RandomBool();
		float impulseY = UKismetMathLibrary::SelectFloat(randomFloat, -randomFloat, randomBool);

		mycharacter->GetCharacterMovement()->AddImpulse(FVector(impulseX, impulseY, 1300.0f), true);
	}
}