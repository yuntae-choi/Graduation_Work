// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "MySnowball.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"

//#define CHECKTRAJECTORY	// 눈덩이가 던져지는 시점에서부터 충돌할 때까지의 궤적 로그 출력

// Sets default values
AMySnowball::AMySnowball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

#ifdef CHECKTRAJECTORY
	// 틱 활성화 (궤적 로그)
	PrimaryActorTick.bCanEverTick = true;
	bCheckTrajectory = false;
#endif

	if (!collisionComponent)
	{
		collisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollisionComponent"));
		collisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		collisionComponent->InitSphereRadius(13.0f);
		collisionComponent->BodyInstance.bNotifyRigidBodyCollision = true;
		collisionComponent->OnComponentHit.AddDynamic(this, &AMySnowball::OnHit);
		collisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		collisionComponent->SetUseCCD(true);

		RootComponent = collisionComponent;
	}

	if (!meshComponent)
	{
		meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SNOWBALL(TEXT("/Game/NonCharacters/snowball1_130.snowball1_130"));
		if (SM_SNOWBALL.Succeeded())
		{
			meshComponent->SetStaticMesh(SM_SNOWBALL.Object);
			meshComponent->BodyInstance.SetCollisionProfileName(TEXT("NoCollision"));
		}
	}

	meshComponent->SetupAttachment(RootComponent);

	if (!projectileMovementComponent)
	{
		projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		projectileMovementComponent->SetUpdatedComponent(collisionComponent);
		projectileMovementComponent->InitialSpeed = 2500.0f;	// 눈덩이 속도 설정
		projectileMovementComponent->MaxSpeed = 2500.0f;
		projectileMovementComponent->bSimulationEnabled = false;	// 생성 후 움직이지 않도록 (눈덩이 릴리즈 시 활성화)
	}

	//crumbleNiagara = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/FX/Snowball/snowballHoudini.snowballHoudini"), nullptr, LOAD_None, nullptr);

	iDamage = 10;
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

#ifdef CHECKTRAJECTORY
	if (bCheckTrajectory) UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
#endif
}

void AMySnowball::Throw_Implementation(FVector Direction, float Speed)
{
	collisionComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	projectileMovementComponent->bSimulationEnabled = true;	// 눈덩이가 움직일 수 있도록 활성화
	projectileMovementComponent->InitialSpeed = Speed;
	projectileMovementComponent->MaxSpeed = Speed;
	projectileMovementComponent->Velocity = (Direction + FVector(0.0f, 0.0f, 0.15f)) * (projectileMovementComponent->InitialSpeed);

	//Delay 함수
	FTimerHandle WaitHandle;
	float WaitTime = 0.01f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			collisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}), WaitTime, false);

#ifdef CHECKTRAJECTORY
	bCheckTrajectory = true;
#endif
}

// Function that is called when the projectile hits something.
void AMySnowball::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	auto MyCharacter = Cast<AMyCharacter>(OtherActor);

	//MYLOG(Warning, TEXT("loc :%f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("loc :%f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z));

	if (nullptr != MyCharacter)
	{
		// 우산과 충돌했는지 확인
		UBoxComponent* boxCollision = Cast<UBoxComponent>(OtherComponent);
		if (boxCollision)
		{
			//UE_LOG(LogTemp, Warning, TEXT("no damage, hit umbrella"));
		}
		else
		{	// 캐릭터와 충돌 시 데미지
			FDamageEvent DamageEvent;
			MyCharacter->TakeDamage(iDamage, DamageEvent, false, this);

			auto BoneName = MyCharacter->GetMesh()->FindClosestBone(GetActorLocation());
			auto ParentBoneName = MyCharacter->GetMesh()->GetParentBone(BoneName);

			MYLOG(Warning, TEXT("%s"), *BoneName.ToString());

			if (BoneName == TEXT("Base-HumanHead") 
				|| ParentBoneName == TEXT("Base-HumanHead"))
				MyCharacter->FreezeHead();

			if (BoneName == TEXT("Base-HumanLForearm")
				|| ParentBoneName == TEXT("Base-HumanLForearm"))
			{
				MyCharacter->FreezeLeftForearm();
				MyCharacter->FreezeLeftUpperarm();
			}

			if (BoneName == TEXT("Base-HumanRForearm")
				|| ParentBoneName == TEXT("Base-HumanRForearm"))
			{
				MyCharacter->FreezeRightForearm();
				MyCharacter->FreezeRightUpperarm();
			}

			if (BoneName == TEXT("Base-HumanLCalf")
				|| ParentBoneName == TEXT("Base-HumanLCalf"))
			{
				MyCharacter->FreezeLeftThigh();
				MyCharacter->FreezeLeftCalf();
			}

			if (BoneName == TEXT("Base-HumanRCalf")
				|| ParentBoneName == TEXT("Base-HumanRCalf"))
			{
				MyCharacter->FreezeRightThigh();
				MyCharacter->FreezeRightCalf();
			}

			if (ParentBoneName == TEXT("Base-HumanPelvis")
				|| BoneName == TEXT("Base-HumanSpine1")
				|| BoneName == TEXT("Base-HumanSpine2")
				|| BoneName == TEXT("Base-HumanRibcage")
				|| ParentBoneName == TEXT("Base-HumanRibcage")
				|| BoneName == TEXT("Base-HumanLUpperarm")
				|| BoneName == TEXT("Base-HumanRUpperarm"))
				MyCharacter->FreezeCenter();
		}
	}
	else
	{
		//MYLOG(Warning, TEXT("no damage"));
	}

	TArray<AActor*> ems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEditorManager::StaticClass(), ems);

	AEditorManager* em = Cast<AEditorManager>(ems[0]);
	em->PlaySnowballCrumbleEffect(GetActorLocation());

	Destroy();
	
#ifdef CHECKTRAJECTORY
	bCheckTrajectory = false;
#endif
}