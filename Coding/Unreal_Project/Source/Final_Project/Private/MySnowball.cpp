// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "MySnowball.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"
#include "Kismet/KismetMathLibrary.h"

//#define CHECKTRAJECTORY	// 눈덩이가 던져지는 시점에서부터 충돌할 때까지의 궤적 로그 출력

TArray<UDecalComponent*> AMySnowball::paints;

// Sets default values
AMySnowball::AMySnowball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	projectileNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("projectileNiagaraComponent"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_PROJECTILE(TEXT("/Game/AssetFolder/MagicSpells_Ice/Effects/Sistems/NS_FrostEnergy_Projectile.NS_FrostEnergy_Projectile"));
	projectileNiagara->SetAsset(NS_PROJECTILE.Object);
	projectileNiagara->SetupAttachment(meshComponent);
	projectileNiagara->SetRelativeLocation(FVector(-30.0f, 0.0f, 0.0f));
	projectileNiagara->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	projectileNiagara->SetVisibility(false);

	trailNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("trailNiagaraComponent"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_TRAIL(TEXT("/Game/AssetFolder/MagicSpells_Ice/Effects/Sistems/NS_Trail_DryIce_Medium.NS_Trail_DryIce_Medium"));
	trailNiagara->SetAsset(NS_TRAIL.Object);
	trailNiagara->SetupAttachment(meshComponent);
	trailNiagara->SetRelativeLocation(FVector(-30.0f, 0.0f, 0.0f));
	trailNiagara->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	trailNiagara->SetVisibility(false);

	hitNiagara = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/AssetFolder/MagicSpells_Ice/Effects/Sistems/NS_Spike_Hit.NS_Spike_Hit"), nullptr, LOAD_None, nullptr);
	
	static ConstructorHelpers::FObjectFinder<USoundWave> HitSoundAsset(TEXT("/Game/Audio/behavior/Hit.Hit"));
	if (HitSoundAsset.Succeeded())
		HitS = HitSoundAsset.Object;

	iDamage = 10;
	iOwnerSessionId = -1;
}

// Called when the game starts or when spawned
void AMySnowball::BeginPlay()
{
	Super::BeginPlay();
	
	startLocation = GetActorLocation();
}

// Called every frame
void AMySnowball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (abs((startLocation - GetActorLocation()).Size()) > 4000.0f)
		Destroy();

	if (bTrailOn)
	{
		//projectileNiagara->SetVisibility(true);
		trailNiagara->SetVisibility(true);
	}

#ifdef CHECKTRAJECTORY
	if (bCheckTrajectory) UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
#endif
}

void AMySnowball::Throw_Implementation(FVector Direction, float Speed)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("loc :%f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z));

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
	auto itemBox = Cast<AItembox>(OtherActor);
	auto umbrella = Cast<UBoxComponent>(OtherComponent);
	auto socketMesh = Cast<UStaticMeshComponent>(OtherComponent);

	FString staticmeshName;
	if (socketMesh)
	{
		socketMesh->GetName(staticmeshName);
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *staticmeshName);
	}

	//projectileMovementComponent->Velocity = FVector(0.0f, 0.0f, 0.0f);
	//collisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshComponent->SetVisibility(false);


	//MYLOG(Warning, TEXT("loc :%f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("loc :%f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z));

	//에디터매니저
	TArray<AActor*> ems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEditorManager::StaticClass(), ems);
	AEditorManager* em = Cast<AEditorManager>(ems[0]);

	//눈 터지는 이펙트
	if (em->snowSplash)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), em->snowSplash, GetActorLocation());
	em->PlaySnowballCrumbleEffect(GetActorLocation());

	auto spawnRot = (GetActorForwardVector() * FVector(1.0f, 1.0f, 0.0f)).Rotation();

	if (hitNiagara) {
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), hitNiagara, GetActorLocation(), spawnRot, FVector(8.0f), true, true, ENCPoolMethod::None, true);
	}

	UGameplayStatics::PlaySoundAtLocation(this, HitS, GetActorLocation());

	if (MyCharacter)
	{
		if (umbrella)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("umbrella")));
		}
		else if (staticmeshName.Compare(FString("jetskiMeshComponent")) == 0)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("jetski")));
		}
		else if (staticmeshName.Compare(FString("bagMeshComponent")) == 0)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("bag")));
		}
		else
		{	// 캐릭터와 충돌 시 데미지
			FDamageEvent DamageEvent;
			MyCharacter->TakeDamage(iDamage, DamageEvent, false, this);

			if (!MyCharacter->GetIsSnowman())
			{
				auto BoneName = MyCharacter->GetMesh()->FindClosestBone(GetActorLocation());
				auto ParentBoneName = MyCharacter->GetMesh()->GetParentBone(BoneName);

				MYLOG(Warning, TEXT("%s"), *BoneName.ToString());

				if (BoneName == TEXT("Base-HumanHead")
					|| ParentBoneName == TEXT("Base-HumanHead")) {
					MyCharacter->SendFreeze(BODDY_HEAD);
				}
				if (BoneName == TEXT("Base-HumanLForearm")
					|| ParentBoneName == TEXT("Base-HumanLForearm"))
				{
					MyCharacter->SendFreeze(BODDY_LEFTHAND);
				}

				if (BoneName == TEXT("Base-HumanRForearm")
					|| ParentBoneName == TEXT("Base-HumanRForearm"))
				{
					MyCharacter->SendFreeze(BODDY_RIGHTHAND);
				}

				if (BoneName == TEXT("Base-HumanLCalf")
					|| ParentBoneName == TEXT("Base-HumanLCalf"))
				{
					MyCharacter->SendFreeze(BODDY_LEFTLEG);
				}

				if (BoneName == TEXT("Base-HumanRCalf")
					|| ParentBoneName == TEXT("Base-HumanRCalf"))
				{
					MyCharacter->SendFreeze(BODDY_RIGHTLEG);
				}

				if (ParentBoneName == TEXT("Base-HumanPelvis")
					|| BoneName == TEXT("Base-HumanSpine1")
					|| BoneName == TEXT("Base-HumanSpine2")
					|| BoneName == TEXT("Base-HumanRibcage")
					|| ParentBoneName == TEXT("Base-HumanRibcage")
					|| BoneName == TEXT("Base-HumanLUpperarm")
					|| BoneName == TEXT("Base-HumanRUpperarm"))
					MyCharacter->SendFreeze(BODDY_CENTER);

			}
		}
	}
	else if (itemBox)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("itembox")));
	}
	else
	{
		if (paints.Num() < 7)
		{
			//눈자국
			FRotator RandomDecalRotation = Hit.Normal.Rotation();
			RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);
			UDecalComponent* comp = UGameplayStatics::SpawnDecalAttached(em->snowPaint, FVector(-35.0f, 50.0f, 50.0f),
				OtherComponent, NAME_None,
				GetActorLocation(), RandomDecalRotation, EAttachLocation::KeepWorldPosition);

			UDecalComponent*& tmp = comp;

			//paints.Add(tmp);
		}
	}

	GetWorld()->GetTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda([&]()
		{
			//눈덩이 삭제
			Destroy();
		}), 1.0f, false);
	
#ifdef CHECKTRAJECTORY
	bCheckTrajectory = false;
#endif
}