// Fill out your copyright notice in the Description page of Project Settings.


#include "SnowballBomb.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"

// Sets default values
ASnowballBomb::ASnowballBomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!collisionComponent)
	{
		collisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollisionComponent"));
		collisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		collisionComponent->InitSphereRadius(10.0f);
		collisionComponent->BodyInstance.bNotifyRigidBodyCollision = true;
		collisionComponent->OnComponentHit.AddDynamic(this, &ASnowballBomb::OnHit);
		collisionComponent->SetUseCCD(true);

		RootComponent = collisionComponent;
	}

	if (!meshComponent)
	{
		meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SNOWBALL(TEXT("/Game/NonCharacters/snowball1.snowball1"));
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
	}

	explosionNiagara = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/FX/NS_Explosion_MV.NS_Explosion_MV"), nullptr, LOAD_None, nullptr);

	iDamage = 10;
	iOwnerSessionId = -1;
}

// Called when the game starts or when spawned
void ASnowballBomb::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASnowballBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASnowballBomb::Throw_Implementation(FVector Direction, float Speed)
{
	projectileMovementComponent->Velocity = (Direction + FVector(0.0f, 0.0f, 0.15f)) * (projectileMovementComponent->InitialSpeed);
}

// Function that is called when the projectile hits something.
void ASnowballBomb::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	auto MyCharacter = Cast<AMyCharacter>(OtherActor);

	if (nullptr != MyCharacter)
	{
		UBoxComponent* boxCollision = Cast<UBoxComponent>(OtherComponent);
		UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(OtherComponent);
		FString staticmeshName;
		if (staticMesh)
		{
			staticMesh->GetName(staticmeshName);
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *staticmeshName);
		}

		if (boxCollision)
		{	// 우산과 충돌한 경우	(눈자국, 몸 어는 이펙트 재생 x 해야함)
			//UE_LOG(LogTemp, Warning, TEXT("no damage, hit umbrella"));
		}
		else if (staticmeshName.Compare(FString("jetskiMeshComponent")) == 0 || staticmeshName.Compare(FString("bagMeshComponent")) == 0)
		{	// 제트스키 or 가방과 충돌한 경우	(눈자국, 몸 어는 이펙트 재생 x 해야함)
			//UE_LOG(LogTemp, Warning, TEXT("no damage, hit jetski or bag"));
		}
		else
		{	// 캐릭터와 충돌 시 데미지
			FDamageEvent DamageEvent;
			MyCharacter->TakeDamage(iDamage, DamageEvent, false, this);
		}
	}
	else
	{
		//MYLOG(Warning, TEXT("no damage"));
	}

	if (Cast<ASnowballBomb>(OtherActor)) return;	// snowball bomb끼리 충돌 시 무시

	if (explosionNiagara) {
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), explosionNiagara, GetActorLocation(), FRotator(1), FVector(1), true, true, ENCPoolMethod::AutoRelease, true);
		//NiagaraComp->SetNiagaraVariableFloat(FString("StrengthCoef"), CoefStrength);
	}

	Destroy();
}