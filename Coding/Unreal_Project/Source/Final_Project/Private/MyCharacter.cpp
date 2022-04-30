// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "MyAnimInstance.h"
#include "MyPlayerController.h"
#include "Snowdrift.h"
#include "Debug.h"
#include "MyPlayerController.h"

const int AMyCharacter::iMaxHP = 390;
const int AMyCharacter::iMinHP = 270;
const int iBeginSlowHP = 300;	// 캐릭터가 슬로우 상태가 되기 시작하는 hp
const int iNormalSpeed = 600;	// 캐릭터 기본 이동속도
const int iSlowSpeed = 400;		// 캐릭터 슬로우 상태 이동속도
const float fChangeSnowmanStunTime = 3.0f;	// 실제값 - 10.0f, 눈사람화 할 때 스턴 시간
const float fStunTime = 3.0f;	// 눈사람이 눈덩이 맞았을 때 스턴 시간
const int iOriginMaxSnowballCount = 10;	// 눈덩이 최대보유량 (초기, 가방x)
const int iOriginMaxMatchCount = 2;	// 성냥 최대보유량 (초기, 가방x)

// 색상별 곰 텍스쳐
FString TextureStringArray[] = {
	TEXT("/Game/Characters/Bear/bear_texture.bear_texture"),
	TEXT("/Game/Characters/Bear/bear_texture_light_red.bear_texture_light_red"),
	TEXT("/Game/Characters/Bear/bear_texture_yellow.bear_texture_yellow"),
	TEXT("/Game/Characters/Bear/bear_texture_light_green.bear_texture_light_green"),
	TEXT("/Game/Characters/Bear/bear_texture_cyan.bear_texture_cyan"),
	TEXT("/Game/Characters/Bear/bear_texture_blue.bear_texture_blue"),
	TEXT("/Game/Characters/Bear/bear_texture_light_gray.bear_texture_light_gray"),
	TEXT("/Game/Characters/Bear/bear_texture_black.bear_texture_black") };

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
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMyCharacter::OnHit);
	GetCapsuleComponent()->BodyInstance.bNotifyRigidBodyCollision = true;
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

	static ConstructorHelpers::FClassFinder<UAnimInstance> BEAR_ANIM(TEXT("/Game/Animations/Bear/BearAnimBP.BearAnimBP_C"));	// _C 붙이기
	if (BEAR_ANIM.Succeeded())
	{
		bearAnim = BEAR_ANIM.Class;
		GetMesh()->SetAnimInstanceClass(BEAR_ANIM.Class);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> SNOWMAN_ANIM(TEXT("/Game/Animations/Snowman/SnowmanAnimBP.SnowmanAnimBP_C"));
	if (SNOWMAN_ANIM.Succeeded())
	{
		snowmanAnim = SNOWMAN_ANIM.Class;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial>BearMaterial(TEXT("/Game/Characters/Bear/M_Bear.M_Bear"));
	if (BearMaterial.Succeeded())
	{
		bearMaterial = BearMaterial.Object;
	}

	// 모든 색상의 곰 텍스쳐 로드해서 저장
	for (int i = 0; i < 8; ++i)
	{
		ConstructorHelpers::FObjectFinder<UTexture>BearTexture(*(TextureStringArray[i]));
		if (BearTexture.Succeeded())
		{
			bearTextureArray.Add(BearTexture.Object);
		}
	}

	static ConstructorHelpers::FObjectFinder<UMaterial>SnowmanMaterial(TEXT("/Game/Characters/Snowman/M_Snowman.M_Snowman"));
	if (SnowmanMaterial.Succeeded())
	{
		snowmanMaterial = SnowmanMaterial.Object;
	}

	GetCharacterMovement()->JumpZVelocity = 800.0f;
	isAttacking = false;

	projectileClass = AMySnowball::StaticClass();

	iSessionId = -1;
	iCurrentHP = iMaxHP;	// 실제 설정값
	//iCurrentHP = iMinHP + 1;	// 디버깅용 - 대기시간 후 눈사람으로 변화

	snowball = nullptr;
	
	iMaxSnowballCount = iOriginMaxSnowballCount;
	iCurrentSnowballCount = 0;	// 실제 설정값
	//iCurrentSnowballCount = 10;	// 디버깅용
	iMaxMatchCount = iOriginMaxMatchCount;
	iCurrentMatchCount = 0;
	bHasUmbrella = false;
	bHasBag = false;

	farmingItem = nullptr;
	bIsFarming = false;
	
	bIsInsideOfBonfire = false;

	//fMatchDuration = 3.0f;
	//match = true;

	iCharacterState = CharacterState::AnimalNormal;
	bIsSnowman = false;
	GetCharacterMovement()->MaxWalkSpeed = iNormalSpeed;	// 캐릭터 이동속도 설정
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 수정 필요 - 캐릭터의 session id가 결정될 때 이 함수가 호출되도록
	SetCharacterMaterial(iSessionId);	// 캐릭터 머티리얼 설정(색상)

	playerController = Cast<APlayerController>(GetController());	// 생성자에서 하면 x (컨트롤러가 생성되기 전인듯)
	localPlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());

	WaitForStartGame();	// 대기시간
}

void AMyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	MYLOG(Warning, TEXT("endplay"));
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerController->GetSocket()->Send_LogoutPacket(iSessionId);
	//PlayerController->GetSocket()->CloseSocket();
	//PlayerController->GetSocket()->StopListen();
 }


// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsSnowman)
	{
		UpdateFarming(DeltaTime);
		UpdateHP();
		UpdateSpeed();
	}
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
	AddMovementInput(GetActorForwardVector(), NewAxisValue);

	if (NewAxisValue != 0.0f) bIsUpDownZero = false;

	if (!bIsUpDownZero)
	{
		AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		PlayerController->SendPlayerInfo(COMMAND_MOVE);
	}

	if (NewAxisValue == 0.0f) bIsUpDownZero = true;
}

void AMyCharacter::LeftRight(float NewAxisValue)
{
	AddMovementInput(GetActorRightVector(), NewAxisValue);

	if (NewAxisValue != 0.0f) bIsLeftRightZero = false;

	if (!bIsLeftRightZero)
	{
		AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		PlayerController->SendPlayerInfo(COMMAND_MOVE);
	}

	if (NewAxisValue == 0.0f) bIsLeftRightZero = true;
}

void AMyCharacter::LookUp(float NewAxisValue)
{
	AddControllerPitchInput(NewAxisValue);

	if (NewAxisValue != 0) bIsLookUpZero = false;

	if (!bIsLookUpZero)
	{
		AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		PlayerController->SendPlayerInfo(COMMAND_MOVE);
	}
}

void AMyCharacter::Turn(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);

	if (NewAxisValue != 0) bIsTurnZero = false;

	if (!bIsTurnZero)
	{
		AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		PlayerController->SendPlayerInfo(COMMAND_MOVE);
	}
}

void AMyCharacter::Attack()
{
	MYLOG(Warning, TEXT("attack"));
	if (bIsSnowman) return;
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	if (iSessionId == PlayerController->iSessionId)  PlayerController->SendPlayerInfo(COMMAND_ATTACK);

#ifdef SINGLEPLAY_DEBUG
	SnowAttack();
#endif
}

void AMyCharacter::SnowAttack()
{
	if (isAttacking) return;
	if (bIsSnowman) return;
	if (iCurrentSnowballCount <= 0) return;	// 눈덩이를 소유하고 있지 않으면 공격 x

	myAnim->PlayAttackMontage();
	isAttacking = true;
	// 디버깅용 - 실제는 주석 해제
	iCurrentSnowballCount -= 1;	// 공격 시 눈덩이 소유량 1 감소
	UpdateUI(UICategory::CurSnowball);

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
	
	if (!bIsSnowman)
	{	// 동물인 경우 체력 감소
#ifdef SINGLEPLAY_DEBUG
		iCurrentHP = FMath::Clamp<int>(iCurrentHP - FinalDamage, iMinHP, iMaxHP);
		UpdateUI(UICategory::HP);	// 변경된 체력으로 ui 갱신

		MYLOG(Warning, TEXT("Actor : %s took Damage : %f, HP : %d"), *GetName(), FinalDamage, iCurrentHP);
#endif
		AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		if (iSessionId == PlayerController->iSessionId)
		{
			PlayerController->SendPlayerInfo(COMMAND_DAMAGE);
		}
	}
	else
	{	// 눈사람인 경우 스턴
		StartStun(fStunTime);
#ifdef SINGLEPLAY_DEBUG
		MYLOG(Warning, TEXT("Actor : %s stunned, HP : %d"), *GetName(), iCurrentHP);
#endif
	}
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
			//던지는 순간 좌표값 보내는 코드
#ifdef MULTIPLAY_DEBUG
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
			
			FVector direction_;
			direction_.X = PlayerController->GetCharactersInfo()->players[iSessionId].fCDx;
			direction_.Y = PlayerController->GetCharactersInfo()->players[iSessionId].fCDy;
			direction_.Z = PlayerController->GetCharactersInfo()->players[iSessionId].fCDz;

			II_Throwable::Execute_Throw(snowball, direction_);
#endif
#ifdef SINGLEPLAY_DEBUG
			FVector cameraLocation;
			FRotator cameraRotation;
			GetActorEyesViewPoint(cameraLocation, cameraRotation);
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());

			II_Throwable::Execute_Throw(snowball, cameraRotation.Vector());
#endif
			snowball = nullptr;
		}

	}
}

void AMyCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
#ifdef MULTIPLAY_DEBUG
	if (!iSessionId == PlayerController->iSessionId || !PlayerController->is_start()) return;
#endif

	auto MySnowball = Cast<AMySnowball>(OtherActor);

	if (nullptr != MySnowball)
	{
		//AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		//if (iSessionId == PlayerController->iSessionId)
		//{
		//	MYLOG(Warning, TEXT("id: %d snowball hit."), iSessionId);

		//	PlayerController->UpdatePlayerInfo(COMMAND_DAMAGE);
		//}

	}
	AMyCharacter* otherCharacter = Cast<AMyCharacter>(OtherActor);
	if (!otherCharacter) return;

	// 자신 - 눈사람, 스턴상태 x
	// 상대 - 동물
	if (bIsSnowman && iCharacterState != CharacterState::SnowmanStunned)
	{
		if (!(otherCharacter->GetIsSnowman()))
		{	// 본인 동물화(부활), 상대 캐릭터 눈사람화(사망)
#ifdef MULTIPLAY_DEBUG
			//bIsSnowman = false;
			PlayerController->SetCharacterState(iSessionId, ST_ANIMAL);
			ChangeAnimal();
			PlayerController->GetSocket()->Send_StatusPacket(ST_ANIMAL, iSessionId);
			UpdateTemperatureState();
			PlayerController->GetSocket()->Send_StatusPacket(ST_SNOWMAN, otherCharacter->iSessionId);
			
			//PlayerController->SetCharacterState(iSessionId, ST_ANIMAL);
			//otherCharacter->ChangeSnowman();
			//PlayerController->SetCharacterState(otherCharacter->iSessionId, ST_SNOWMAN);
#endif
#ifdef SINGLEPLAY_DEBUG
			ChangeAnimal();
			otherCharacter->ChangeSnowman();
#endif
			UE_LOG(LogTemp, Warning, TEXT("%s catch %s"), *GetName(), *(otherCharacter->GetName()));
			return;
		}
	}
	/*else if (!bIsSnowman && otherCharacter->GetIsSnowman())
	{
		bIsSnowman = true;
		PlayerController->SetCharacterState(iSessionId, ST_SNOWMAN);
		ChangeSnowman();
		if (iSessionId == PlayerController->iSessionId && PlayerController->is_start())
			PlayerController->GetSocket()->Send_StatusPacket(ST_SNOWMAN);
		
	}*/

	//auto MyCharacter = Cast<AMyCharacter>(OtherActor);

	//if (nullptr != MyCharacter)
	//{
	//	FDamageEvent DamageEvent;
	//	MyCharacter->TakeDamage(iDamage, DamageEvent, false, this);
	//}
}

void AMyCharacter::StartFarming()
{
	if (!IsValid(farmingItem)) return;	//오버랩하면 바로 넣어줌
	if (bIsSnowman) return;

	if (Cast<ASnowdrift>(farmingItem))
	{
		if (iCurrentSnowballCount >= iMaxSnowballCount) return;	// 눈덩이 최대보유량 이상은 눈 무더기 파밍 못하도록
		bIsFarming = true;
	}
	else if (Cast<AItembox>(farmingItem))
	{
		AItembox* itembox = Cast<AItembox>(farmingItem);
		AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		switch (itembox->GetItemboxState())
		{
		case ItemboxState::Closed:
			itembox->SetItemboxState(ItemboxState::Opening);
			break;
		case ItemboxState::Opened:
			// 아이템박스에서 내용물 파밍에 성공하면 아이템박스에서 아이템 제거 (박스는 그대로 유지시킴)
			if (GetItem(itembox->GetItemType())) { 
				MYLOG(Warning, TEXT("item %d"), itembox->GetItemType());
				//PlayerController->UpdateFarming(itembox->GetItemType());
				itembox->DeleteItem(); 			
			}
			break;
		// 아이템박스가 열리는 중이거나 비어있는 경우
		case ItemboxState::Opening:
			break;
		case ItemboxState::Empty:
			break;
		default:
			break;
		}
	}
}

bool AMyCharacter::GetItem(int itemType)
{
	switch (itemType) {
	case ItemTypeList::Match:
		if (iCurrentMatchCount >= iMaxMatchCount) return false;	// 성냥 최대보유량을 넘어서 파밍하지 못하도록
		iCurrentMatchCount += 1;
		UpdateUI(UICategory::CurMatch);
		return true;
		break;
	case ItemTypeList::Umbrella:
		if (bHasUmbrella) return false;	// 우산을 소유 중이면 우산 파밍 못하도록
		bHasUmbrella = true;
		UpdateUI(UICategory::HasUmbrella);
		return true;
		break;
	case ItemTypeList::Bag:
		if (bHasBag) return false;	// 가방을 소유 중이면 가방 파밍 못하도록
		bHasBag = true;
		iMaxSnowballCount += 5;	// 눈덩이 10 -> 15 까지 보유 가능
		iMaxMatchCount += 1;	// 성냥 2 -> 3 까지 보유 가능
		UpdateUI(UICategory::HasBag);
		UpdateUI(UICategory::MaxSnowballAndMatch);
		return true;
		break;
	default:
		return false;
		break;
	}
}

void AMyCharacter::EndFarming()
{
	if (bIsSnowman) return;

	if (IsValid(farmingItem))
	{
		if (Cast<ASnowdrift>(farmingItem))
		{	

			if (iCurrentSnowballCount >= iMaxSnowballCount) return;
			
			// F키로 눈 무더기 파밍 중 F키 release 시 눈 무더기 duration 초기화
			ASnowdrift* snowdrift = Cast<ASnowdrift>(farmingItem);
			snowdrift->SetFarmDuration(ASnowdrift::fFarmDurationMax);
			bIsFarming = false;
		}
	}
}

void AMyCharacter::UpdateFarming(float deltaTime)
{
	if (!bIsFarming) return;
	if (!IsValid(farmingItem)) return;

	ASnowdrift* snowdrift = Cast<ASnowdrift>(farmingItem);
	if (snowdrift)
	{	// 눈 무더기 duration 만큼 F키를 누르고 있으면 캐릭터의 눈덩이 추가 
		float lastFarmDuration = snowdrift->GetFarmDuration();
		float newFarmDuration = lastFarmDuration - deltaTime;
		snowdrift->SetFarmDuration(newFarmDuration);

		if (newFarmDuration <= 0)
		{
			//눈덩이 파밍 시 서버 전송
#ifdef MULTIPLAY_DEBUG
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
			PlayerController->GetSocket()->Send_ItemPacket(ITEM_SNOW, snowdrift->GetId());
#endif



			iCurrentSnowballCount += ASnowdrift::iNumOfSnowball; //서버에서 처리
			iCurrentSnowballCount = FMath::Clamp<int>(iCurrentSnowballCount, 0, iMaxSnowballCount);	//서버에서 처리
			UpdateUI(UICategory::CurSnowball);
			snowdrift->Destroy(); //서버에서 아이디 반환시 처리
			snowdrift = nullptr;
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
	case CharacterState::AnimalNormal:
		if (iCurrentHP <= iBeginSlowHP)
		{
			iCharacterState = CharacterState::AnimalSlow;
			GetCharacterMovement()->MaxWalkSpeed = iSlowSpeed;
		}
		break;
	case CharacterState::AnimalSlow:
		if (iCurrentHP > iBeginSlowHP)
		{
			iCharacterState = CharacterState::AnimalNormal;
			GetCharacterMovement()->MaxWalkSpeed = iNormalSpeed;
		}
		break;
	default:
		break;
	}
}

void AMyCharacter::ChangeSnowman()
{
	bIsSnowman = true;

	// 스켈레탈메시, 애니메이션 블루프린트 변경
	myAnim->SetDead();
	GetMesh()->SetSkeletalMesh(snowman);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetAnimInstanceClass(snowmanAnim);
	SetCharacterMaterial();	// 눈사람으로 머티리얼 변경할 때는 색상 필요 x (디폴트값으로)

	iCurrentHP = iMinHP;
	GetWorldTimerManager().ClearTimer(temperatureHandle);	// 기존에 실행중이던 체온 증감 핸들러 초기화 (체온 변화하지 않도록)
#ifdef SINGLEPLAY_DEBUG
	UpdateTemperatureState();
	UpdateUI(UICategory::HP);	// 변경된 체력으로 ui 갱신
#endif

	GetCharacterMovement()->MaxWalkSpeed = iSlowSpeed;	// 눈사람의 이동속도는 슬로우 상태인 캐릭터와 동일하게 설정
	
	StartStun(fChangeSnowmanStunTime);

	ResetHasItems();

	isAttacking = false;	// 공격 도중에 상태 변할 시 발생하는 오류 방지
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
	if (bIsSnowman) return;

#ifdef SINGLEPLAY_DEBUG
	GetWorldTimerManager().ClearTimer(temperatureHandle);	// 기존에 실행중이던 핸들러 초기화
#endif
	//if (match)
	//{
	//	GetWorldTimerManager().SetTimer(temperatureHandle, this, &AMyCharacter::UpdateTemperatureByMatch, 1.0f, true);
	//}
	//else
	//{
		if (bIsInsideOfBonfire)
		{	// 모닥불 내부인 경우 초당 체온 증가 (초당 호출되는 람다함수)
#ifdef SINGLEPLAY_DEBUG
			GetWorldTimerManager().SetTimer(temperatureHandle, FTimerDelegate::CreateLambda([&]()
				{
					iCurrentHP += ABonfire::iHealAmount;
					iCurrentHP = FMath::Clamp<int>(iCurrentHP, iMinHP, iMaxHP);
					UpdateUI(UICategory::HP);	// 변경된 체력으로 ui 갱신
				}), 1.0f, true);
#endif
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
			if (iSessionId == PlayerController->iSessionId && PlayerController->is_start())
				PlayerController->GetSocket()->Send_StatusPacket(ST_INBURN, iSessionId);
		}
		else
		{	// 모닥불 외부인 경우 초당 체온 감소 (초당 호출되는 람다함수)
#ifdef SINGLEPLAY_DEBUG
			GetWorldTimerManager().SetTimer(temperatureHandle, FTimerDelegate::CreateLambda([&]()
				{
					iCurrentHP -= ABonfire::iDamageAmount;
					iCurrentHP = FMath::Clamp<int>(iCurrentHP, iMinHP, iMaxHP);
					UpdateUI(UICategory::HP);	// 변경된 체력으로 ui 갱신
				}), 1.0f, true);
#endif
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
			if (iSessionId == PlayerController->iSessionId && PlayerController->is_start())
				PlayerController->GetSocket()->Send_StatusPacket(ST_OUTBURN, iSessionId);
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

void AMyCharacter::StartStun(float waitTime)
{
	//if (!playerController) return;	// 다른 플레이어의 캐릭터는 플레이어 컨트롤러가 존재 x?

	if (iCharacterState == CharacterState::SnowmanStunned)
	{	// 스턴 상태에서 또 맞으면 기존에 실행중이던 stunhandle 초기화 (핸들러 새로 갱신하도록)
		GetWorldTimerManager().ClearTimer(stunHandle);
	}

	// 플레이어의 입력을 무시하도록 (움직일 수 없음, 시야도 고정, 상태 - Stunned)
	iCharacterState = CharacterState::SnowmanStunned;
	DisableInput(playerController);
	GetMesh()->bPauseAnims = true;	// 캐릭터 애니메이션도 정지

	EndStun(waitTime);	// waitTime이 지나면 stun이 끝나도록
}

void AMyCharacter::EndStun(float waitTime)
{
	//if (!playerController) return;

	// x초 후에 다시 입력을 받을 수 있도록 (움직임과 시야 제한 해제, 상태 - Snowman)
	GetWorld()->GetTimerManager().SetTimer(stunHandle, FTimerDelegate::CreateLambda([&]()
		{
			iCharacterState = CharacterState::SnowmanNormal;
			EnableInput(playerController);
			GetMesh()->bPauseAnims = false;
		}), waitTime, false);
}

void AMyCharacter::ResetHasItems()
{
	iCurrentSnowballCount = 0;
	iMaxSnowballCount = iOriginMaxSnowballCount;
	iCurrentMatchCount = 0;
	iMaxMatchCount = iOriginMaxMatchCount;
	bHasUmbrella = false;
	bHasBag = false;

	UpdateUI(UICategory::AllOfUI);
}

void AMyCharacter::ChangeAnimal()
{
	bIsSnowman = false;

	// 스켈레탈메시, 애니메이션 블루프린트 변경
	GetMesh()->SetSkeletalMesh(bear);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetAnimInstanceClass(bearAnim);
	SetCharacterMaterial(iSessionId);

	myAnim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	MYCHECK(nullptr != myAnim);
	myAnim->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnAttackMontageEnded);
	//iCurrentSnowballCount = 10;	// 디버깅용
	iCurrentHP = iMaxHP;
	GetWorldTimerManager().ClearTimer(temperatureHandle);
	UpdateUI(UICategory::CurSnowball);

#ifdef SINGLEPLAY_DEBUG
	UpdateTemperatureState();
	UpdateUI(UICategory::HP);	// 변경된 체력으로 ui 갱신
#endif

	GetCharacterMovement()->MaxWalkSpeed = iNormalSpeed;

	iCharacterState = CharacterState::AnimalNormal;

	isAttacking = false;	// 공격 도중에 상태 변할 시 발생하는 오류 방지
}

void AMyCharacter::SetCharacterMaterial(int id)
{
	if (id < 0) id = 0;	// id가 유효하지 않은 경우 (싱글플레이)
	if (!bIsSnowman)
	{	// 곰 머티리얼로 변경, 본인 색상의 곰 텍스쳐 적용
		GetMesh()->SetMaterial(0, bearMaterial);
		dynamicMaterialInstance = GetMesh()->CreateDynamicMaterialInstance(0);
		dynamicMaterialInstance->SetTextureParameterValue(FName("Tex"), bearTextureArray[id]);	// 본인 색상의 곰 텍스쳐 사용
	}
	else
	{	// 눈사람 머티리얼로 변경
		GetMesh()->SetMaterial(0, snowmanMaterial);
	}
}

void AMyCharacter::UpdateUI(int uiCategory)
{
#ifdef MULTIPLAY_DEBUG
	if (iSessionId != localPlayerController->iSessionId) return;	// 로컬플레이어인 경우만 update
#endif
	switch (uiCategory)
	{
	case UICategory::HP:
		localPlayerController->CallDelegateUpdateHP();	// 체력 ui 갱신
		break;
	case UICategory::CurSnowball:
		localPlayerController->CallDelegateUpdateCurrentSnowballCount();	// CurSnowball ui 갱신
		break;
	case UICategory::CurMatch:
		localPlayerController->CallDelegateUpdateCurrentMatchCount();	// CurMatch ui 갱신
		break;
	case UICategory::MaxSnowballAndMatch:
		localPlayerController->CallDelegateUpdateMaxSnowballAndMatchCount();	// MaxSnowballAndMatch ui 갱신
		break;
	case UICategory::HasUmbrella:
		localPlayerController->CallDelegateUpdateHasUmbrella();	// HasUmbrella ui 갱신
		break;
	case UICategory::HasBag:
		localPlayerController->CallDelegateUpdateHasBag();	// HasBag ui 갱신
		break;
	case UICategory::AllOfUI:
		localPlayerController->CallDelegateUpdateAllOfUI();	// 모든 캐릭터 ui 갱신
		break;
	default:
		break;
	}
}