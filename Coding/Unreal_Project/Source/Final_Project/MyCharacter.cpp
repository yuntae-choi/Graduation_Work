// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "MyAnimInstance.h"
#include "MyItem.h"
#include "MyPlayerController.h"
#include "Snowdrift.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	check(camera != nullptr);
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(80.0f);
	GetCapsuleComponent()->SetCapsuleRadius(38.0f);
	springArm->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	camera->SetupAttachment(springArm);

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MyCharacter"));
	GetCapsuleComponent()->SetCapsuleHalfHeight(74.0f);
	GetCapsuleComponent()->SetCapsuleRadius(37.0f);
	GetCapsuleComponent()->SetUseCCD(true);
	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMyCharacter::OnHit);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -74.0f), FRotator(0.0f, -90.0f, 0.0f));
	springArm->TargetArmLength = 220.0f;
	springArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator::ZeroRotator);
	springArm->bUsePawnControlRotation = true;
	springArm->bInheritPitch = true;
	springArm->bInheritRoll = true;
	springArm->bInheritYaw = true;
	springArm->bDoCollisionTest = true;
	springArm->SocketOffset.Y = 35.0f;
	springArm->SocketOffset.Z = 35.0f;
	bUseControllerRotationYaw = true;

	bear = CreateDefaultSubobject<USkeletalMesh>(TEXT("BEAR"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_BEAR(TEXT("/Game/Characters/Bear/bear.bear"));
	if (SK_BEAR.Succeeded())
	{
		bear = SK_BEAR.Object;
		GetMesh()->SetSkeletalMesh(bear);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	snowman = CreateDefaultSubobject<USkeletalMesh>(TEXT("SNOWMAN"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SNOWMAN(TEXT("/Game/Characters/Snowman/snowman.snowman"));
	if (SK_SNOWMAN.Succeeded())
	{
		snowman = SK_SNOWMAN.Object;
	}
	
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> BEAR_ANIM(TEXT("/Game/Animations/Bear/BearAnimBP.BearAnimBP_C"));
	if (BEAR_ANIM.Succeeded())
	{
		bearAnim = BEAR_ANIM.Class;
		GetMesh()->SetAnimInstanceClass(BEAR_ANIM.Class);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> SNOWMAN_ANIM(TEXT("/Game/Animations/Snowman/SnowMan_AnimBP.SnowMan_AnimBP_C"));
	if (SNOWMAN_ANIM.Succeeded())
	{
		snowmanAnim = SNOWMAN_ANIM.Class;
	}

	GetCharacterMovement()->JumpZVelocity = 800.0f;
	isAttacking = false;

	projectileClass = AMySnowball::StaticClass();

	snowball = nullptr;
	iSessionID = 0;
	fMaxHP = 120.0f;
	fCurrentHP = fMaxHP;
	fAttack = 10.0f;
	iMaxSnowballCount = 3;
	iCurrentSnowballCount = 0; 
	iPlusMaxSnowballCountByABag = 2;
	bHasUmbrella = false;
	bHasBag = false;
	iMaxMatchCount = 3;
	iCurrentMatchCount = 0;
	farmingItem = NULL;
	bIsFarming = false;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateFarming(DeltaTime);
}

void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	myAnim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	MYCHECK(nullptr != myAnim);

	myAnim->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnAttackMontageEnded);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMyCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMyCharacter::Turn);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AMyCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Farming"), EInputEvent::IE_Pressed, this, &AMyCharacter::StartFarming);
	PlayerInputComponent->BindAction(TEXT("Farming"), EInputEvent::IE_Released, this, &AMyCharacter::EndFarming);
}

bool AMyCharacter::CanSetItem()
{
	//return (nullptr == CurrentItem);
	return 0;
}

void AMyCharacter::SetItem(AMyItem* NewItem)
{
	//MYCHECK(nullptr != NewItem && nullptr == CurrentItem);
	//FName ItemSocket(TEXT("hand_rSocket"));
	//auto CurItem = GetWorld()->SpawnActor<AMyItem>(FVector::ZeroVector, FRotator::ZeroRotator);
	//if (nullptr != NewItem)
	//{
	//	NewItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemSocket);
	//	NewItem->SetOwner(this);
	//	CurrentItem = NewItem;
	//}
}

void AMyCharacter::UpDown(float NewAxisValue)
{
	if (NewAxisValue != 0)
	{
		AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		PlayerController->UpdatePlayerInfo(COMMAND_MOVE);
	}
	AddMovementInput(GetActorForwardVector(), NewAxisValue);
}

void AMyCharacter::LeftRight(float NewAxisValue)
{
	if (NewAxisValue != 0)
	{
		AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		PlayerController->UpdatePlayerInfo(COMMAND_MOVE);
	}
	AddMovementInput(GetActorRightVector(), NewAxisValue);
}

void AMyCharacter::LookUp(float NewAxisValue)
{
	AddControllerPitchInput(NewAxisValue);
}

void AMyCharacter::Turn(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);
}

void AMyCharacter::Attack()
{
	if (isAttacking) return;
	//if (iCurrentSnowballCount <= 0) return;	// 눈덩이를 소유하고 있지 않으면 공격 x

	myAnim->PlayAttackMontage();
	isAttacking = true;
	// 디버깅용 - 실제는 주석 해제
	//iSnowballCount -= 1;	// 공격 시 눈덩이 소유량 1 감소

	// Attempt to fire a projectile.
	if (projectileClass)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			FTransform SnowballSocketTransform = GetMesh()->GetSocketTransform(TEXT("SnowballSocket"));
			snowball = World->SpawnActor<AMySnowball>(projectileClass, SnowballSocketTransform, SpawnParams);
			FAttachmentTransformRules atr = FAttachmentTransformRules(
				EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
			snowball->AttachToComponent(GetMesh(), atr, TEXT("SnowballSocket"));
			snowball->fAttack = fAttack;
		}
	}
}
//void AMyCharacter::Attack()
//{
//	if (isAttacking) return;
//
//
//	myAnim->PlayAttackMontage();
//	isAttacking = true;
//
//	// Attempt to fire a projectile.
//	if (projectileClass)
//	{
//		// Get the camera transform.
//		FVector CameraLocation;
//		FRotator CameraRotation;
//		GetActorEyesViewPoint(CameraLocation, CameraRotation);
//
//		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
//		MuzzleOffset.Set(100.0f, 50.0f, -100.0f);
//
//		FVector MuzzleLocation = CameraLocation+FTransform(CameraRotation).TransformVector(MuzzleOffset);
//		FRotator MuzzleRotation = CameraRotation;
//
//		MuzzleRotation.Pitch += 10.0f; 
//		UWorld* World = GetWorld();
//
//		if (World)
//		{
//			FActorSpawnParameters SpawnParams;
//			SpawnParams.Owner = this; 
//			SpawnParams.Instigator = GetInstigator();
//			AMySnow* Projectile = World->SpawnActor<AMySnow>(projectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
//			if (Projectile)
//			{
//				FVector LaunchDirection = MuzzleRotation.Vector(); 
//				Projectile->FireInDirection(LaunchDirection);
//				Projectile->SetAttack(CharacterStat->GetAttack());
//			}
//		}
//	}
//}

void AMyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	isAttacking = false;
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	SetDamage(FinalDamage);

	MYLOG(Warning, TEXT("Actor : %s took Damage : %f, HP : %f"), *GetName(), FinalDamage, fCurrentHP);

	return FinalDamage;
}

void AMyCharacter::ReleaseSnowball()
{
	if (IsValid(snowball))
	{
		FDetachmentTransformRules dtr = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
		snowball->DetachFromActor(dtr);

		if (snowball->GetClass()->ImplementsInterface(UI_Throwable::StaticClass()))
		{
			FVector cameraLocation;
			FRotator cameraRotation;
			GetActorEyesViewPoint(cameraLocation, cameraRotation);

			II_Throwable::Execute_Throw(snowball, cameraRotation.Vector());
			snowball = NULL;
		}

	}
}

void AMyCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//auto MySnowball = Cast<AMySnowball>(OtherActor);

	//if (nullptr != MySnowball)
	//{
	//	MYLOG(Warning, TEXT("snowball hit."));
	//}
}

void AMyCharacter::SetDamage(float NewDamage)
{
	//SetHP(FMath::Clamp<float>(fCurrentHP - NewDamage, 0.0f, fMaxHP));
	fCurrentHP = FMath::Clamp<float>(fCurrentHP - NewDamage, 0.0f, fMaxHP);
	//OnHPChanged.Broadcast();
	if (fCurrentHP < KINDA_SMALL_NUMBER)
	{
		fCurrentHP = 0.0f;
		myAnim->SetDead();
		GetMesh()->SetSkeletalMesh(snowman);
		GetMesh()->SetAnimInstanceClass(snowmanAnim);
	}
}

void AMyCharacter::StartFarming()
{
	if (IsValid(farmingItem))
	{
		ASnowdrift* snowdrift = Cast<ASnowdrift>(farmingItem);
		if (snowdrift)
		{
			bIsFarming = true;
		}
	}
}

void AMyCharacter::EndFarming()
{
	if (IsValid(farmingItem))
	{
		ASnowdrift* snowdrift = Cast<ASnowdrift>(farmingItem);
		if (snowdrift)
		{	// F키로 눈 무더기 파밍 중 F키 release 시 눈 무더기 duration 초기화
			snowdrift->SetFarmDuration(ASnowdrift::fFarmDurationMax);
			bIsFarming = false;
		}
	}
}

void AMyCharacter::UpdateFarming(float deltaTime)
{
	if (bIsFarming)
	{
		if (IsValid(farmingItem))
		{
			ASnowdrift* snowdrift = Cast<ASnowdrift>(farmingItem);
			if (snowdrift)
			{	// 눈 무더기 duration 만큼 F키를 누르고 있으면 캐릭터의 눈덩이 추가 
				float lastFarmDuration = snowdrift->GetFarmDuration();
				float newFarmDuration = lastFarmDuration - deltaTime;
				snowdrift->SetFarmDuration(newFarmDuration);

				if (newFarmDuration <= 0)
				{
					iCurrentSnowballCount += 10;
					if (iCurrentSnowballCount >= iMaxSnowballCount)
					{
						iCurrentSnowballCount = iMaxSnowballCount;
					}
					snowdrift->Destroy();
				}
			}
		}
	}
}