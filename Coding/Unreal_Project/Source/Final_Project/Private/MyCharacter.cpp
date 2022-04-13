// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "MyAnimInstance.h"
#include "MyItem.h"
#include "MyPlayerController.h"
#include "Snowdrift.h"

const int AMyCharacter::iMaxHP = 390;
const int AMyCharacter::iMinHP = 270;
const int iBeginSlowHP = 300;	// 캐릭터가 슬로우 상태가 되기 시작하는 hp
const int iNormalSpeed = 600;	// 캐릭터 기본 이동속도
const int iSlowSpeed = 400;		// 캐릭터 슬로우 상태 이동속도
// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	check(camera != nullptr);

	springArm->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	camera->SetupAttachment(springArm);

	GetCapsuleComponent()->SetCapsuleHalfHeight(74.0f);
	GetCapsuleComponent()->SetCapsuleRadius(37.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MyCharacter"));
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
	springArm->SocketOffset.Y = 30.0f;
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
	iCurrentHP = iMaxHP;
	iMaxSnowballCount = 3;
	iCurrentSnowballCount = 0; 
	bHasUmbrella = false;
	bHasBag = false;
	iMaxMatchCount = 3;
	iCurrentMatchCount = 0;
	farmingItem = nullptr;
	bIsFarming = false;
	bIsInsideOfBonfire = false;	// 초기값 : true로 설정해야함,  캐릭터 초기 생성위치 모닥불 내부여야 함

	//fMatchDuration = 3.0f;
	//match = true;

	iCharacterState = CharacterState::Normal;
	GetCharacterMovement()->MaxWalkSpeed = iNormalSpeed;	// 캐릭터 이동속도 설정
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	WaitForStartGame();	// 대기시간
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateFarming(DeltaTime);
	UpdateHP();
	UpdateSpeed();
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
		}
	}
}

void AMyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	isAttacking = false;
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	iCurrentHP = FMath::Clamp<int>(iCurrentHP - FinalDamage, iMinHP, iMaxHP);

	MYLOG(Warning, TEXT("Actor : %s took Damage : %f, HP : %d"), *GetName(), FinalDamage, iCurrentHP);

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
			snowball = nullptr;
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

void AMyCharacter::StartFarming()
{
	if (IsValid(farmingItem))
	{
		if (Cast<ASnowdrift>(farmingItem))
		{
			bIsFarming = true;
		}
		else if (Cast<AItembox>(farmingItem))
		{
			AItembox* itembox = Cast<AItembox>(farmingItem);
			switch (itembox->GetItemboxState())
			{
			case ItemboxState::Closed:
				itembox->SetItemboxState(ItemboxState::Opening);
				break;
			case ItemboxState::Opened:
				// 아이템박스에서 내용물 파밍에 성공하면 아이템박스에서 아이템 제거 (박스는 그대로 유지시킴)
				if (GetItem(itembox->GetItemType())) { itembox->DeleteItem(); }
				break;
			default:
				break;
			}
		}
	}
}

bool AMyCharacter::GetItem(int itemType)
{
	switch (itemType) {
	case ItemTypeList::Match:
		return true;
		break;
	case ItemTypeList::Umbrella:
		if (bHasUmbrella) return false;	// 우산을 소유 중이면 우산 파밍 못하도록
		bHasUmbrella = true;
		return true;
		break;
	case ItemTypeList::Bag:
		if (bHasBag) return false;	// 가방을 소유 중이면 가방 파밍 못하도록
		bHasBag = true;
		// 눈덩이 최대 보유량 증가 (성냥도 증가하도록?)
		return true;
		break;
	default:
		return false;
		break;
	}
}

void AMyCharacter::EndFarming()
{
	if (IsValid(farmingItem))
	{
		if (Cast<ASnowdrift>(farmingItem))
		{	// F키로 눈 무더기 파밍 중 F키 release 시 눈 무더기 duration 초기화
			ASnowdrift* snowdrift = Cast<ASnowdrift>(farmingItem);
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
					iCurrentSnowballCount = iCurrentSnowballCount > iMaxSnowballCount ? iMaxSnowballCount : iCurrentSnowballCount;
					snowdrift->Destroy();
				}
			}
		}
	}
}

void AMyCharacter::UpdateHP()
{
	if (iCurrentHP <= iMinHP)
	{
		ChangeSnowman();
	}
}

void AMyCharacter::UpdateSpeed()
{
	switch (iCharacterState) {
	case CharacterState::Normal:
		if (iCurrentHP <= iBeginSlowHP)
		{
			iCharacterState = CharacterState::Slow;
			GetCharacterMovement()->MaxWalkSpeed = iSlowSpeed;
		}
		break;
	case CharacterState::Slow:
		if (iCurrentHP > iBeginSlowHP)
		{
			iCharacterState = CharacterState::Normal;
			GetCharacterMovement()->MaxWalkSpeed = iNormalSpeed;
		}
		break;
	case CharacterState::Snowman:
		break;
	default:
		break;
	}
}

void AMyCharacter::ChangeSnowman()
{
	iCurrentHP = iMinHP;
	GetWorldTimerManager().ClearTimer(temperatureHandle);	// 기존에 실행중이던 체온 증감 핸들러 초기화 (체온 변화하지 않도록)
	myAnim->SetDead();
	GetMesh()->SetSkeletalMesh(snowman);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetAnimInstanceClass(snowmanAnim);

	GetCharacterMovement()->MaxWalkSpeed = iSlowSpeed;	// 눈사람의 이동속도는 슬로우 상태인 캐릭터와 동일하게 설정
}

void AMyCharacter::WaitForStartGame()
{
	//Delay 함수
	FTimerHandle WaitHandle;
	float WaitTime = 3.0f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			UpdateTemperatureState();
		}), WaitTime, false);
}

void AMyCharacter::UpdateTemperatureState()
{
	GetWorldTimerManager().ClearTimer(temperatureHandle);	// 기존에 실행중이던 핸들러 초기화
	//if (match)
	//{
	//	GetWorldTimerManager().SetTimer(temperatureHandle, this, &AMyCharacter::UpdateTemperatureByMatch, 1.0f, true);
	//}
	//else
	//{
		if (bIsInsideOfBonfire)
		{	// 모닥불 내부인 경우 초당 체온 증가 (초당 호출되는 람다함수)
			GetWorldTimerManager().SetTimer(temperatureHandle, FTimerDelegate::CreateLambda([&]()
				{
					iCurrentHP += 10;
					iCurrentHP = iCurrentHP > iMaxHP ? iMaxHP : iCurrentHP;
				}), 1.0f, true);
		}
		else
		{	// 모닥불 외부인 경우 초당 체온 감소 (초당 호출되는 람다함수)
			GetWorldTimerManager().SetTimer(temperatureHandle, FTimerDelegate::CreateLambda([&]()
				{
					iCurrentHP -= 1;
				}), 1.0f, true);
		}
	//}
}

//void AMyCharacter::UpdateTemperatureByMatch()
//{
//	fMatchDuration -= 1.0f;
//	iCurrentHP += 1.0f;
//
//	if (fMatchDuration <= 0.0f)
//	{
//		match = false;
//		fMatchDuration = 3.0f;
//		UpdateTemperatureState();
//	}
//}