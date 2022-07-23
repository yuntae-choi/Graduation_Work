// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "MyAnimInstance.h"
#include "MyPlayerController.h"
#include "Snowdrift.h"
#include "Icedrift.h"
#include "Debug.h"
#include "Kismet/KismetMathLibrary.h"
#include "UmbrellaAnimInstance.h"
#include "EmptyActor.h"
#include "GameFramework/HUD.h"
#include "Jetski.h"

const int AMyCharacter::iMaxHP = 390;
const int AMyCharacter::iMinHP = 270;
const int AMyCharacter::iBeginSlowHP = 300;	// 캐릭터가 슬로우 상태가 되기 시작하는 hp
const int iNormalSpeed = 600;	// 캐릭터 기본 이동속도
const int iSlowSpeed = 400;		// 캐릭터 슬로우 상태 이동속도
const int iJetskiSpeed = 1200;		// Jetski 탑승 시 이동속도
const float fChangeSnowmanStunTime = 3.0f;	// 실제값 - 10.0f, 눈사람화 할 때 스턴 시간
const float fStunTime = 3.0f;	// 눈사람이 눈덩이 맞았을 때 스턴 시간
const int iOriginMaxSnowballCount = 10;	// 눈덩이 최대보유량 (초기, 가방x)
const int iOriginMaxIceballCount = 10;	// 아이스볼 최대보유량 (초기, 가방x)
const int iOriginMaxMatchCount = 2;	// 성냥 최대보유량 (초기, 가방x)
const int iNumOfWeapons = 2;	// 무기 종류 수
const int iNumOfProjectiles = 2;	// 발사체 종류 수
const float fAimingTime = 0.2f;		// 조준하는 데 걸리는 시간 (카메라 전환만, 애니메이션은 따로)
const float fThrowPower = 700.0f;
const float fMaxChargingTime = 5.0f;	// 최대 차징 시간
const float fSnowballInitialSpeed = 2000.0f;	// 눈덩이 초기 속도

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

const int iNumOfPathSpline = 35;
FString SplineStringArray[] = {
	TEXT("spline1"), TEXT("spline2"),TEXT("spline3"),TEXT("spline4"),TEXT("spline5"),TEXT("spline6"), TEXT("spline7"),
	TEXT("spline8"),TEXT("spline9"),TEXT("spline10"),TEXT("spline11"),TEXT("spline12"), TEXT("spline13"),TEXT("spline14"),
	TEXT("spline15"), TEXT("spline16"),TEXT("spline17"),TEXT("spline18"),TEXT("spline19"), TEXT("spline20"),TEXT("spline21"),
	TEXT("spline22"), TEXT("spline23"),TEXT("spline24"),TEXT("spline25"),TEXT("spline26"), TEXT("spline27"),TEXT("spline28"),
	TEXT("spline29"), TEXT("spline30"),TEXT("spline31"),TEXT("spline32"),TEXT("spline33"), TEXT("spline34"),TEXT("spline35")
};

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
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -77.0f), FRotator(0.0f, -90.0f, 0.0f));
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

	springArm2 = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM2"));
	springArm2->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	springArm2->TargetArmLength = 200.0f;
	springArm2->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(0.0f, -30.0f, 0.0f));
	springArm2->bUsePawnControlRotation = true;
	springArm2->bInheritPitch = true;
	springArm2->bInheritRoll = true;
	springArm2->bInheritYaw = true;
	springArm2->bDoCollisionTest = true;
	springArm2->SocketOffset.Y = 90.0f;
	springArm2->SocketOffset.Z = 35.0f;

	springArm3 = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM3"));
	springArm3->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	springArm3->TargetArmLength = 220.0f;
	springArm3->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 80.0f), FRotator::ZeroRotator);
	springArm3->bUsePawnControlRotation = true;
	springArm3->bInheritPitch = true;
	springArm3->bInheritRoll = true;
	springArm3->bInheritYaw = true;
	springArm3->bDoCollisionTest = true;

	camera3 = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA3"));
	check(camera3 != nullptr);
	camera3->SetupAttachment(springArm3);

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

	//static ConstructorHelpers::FObjectFinder<UMaterialInstance>BearMaterial(TEXT("/Game/Characters/Bear/M_Bear_Inst.M_Bear_Inst"));
	//if (BearMaterial.Succeeded())
	//{
	//	bearMaterial = BearMaterial.Object;
	//}

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

	if (!shotgunMeshComponent)
	{
		shotgunMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("shotgunMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SHOTGUN(TEXT("/Game/NonCharacters/Shotgun_SM.Shotgun_SM"));
		if (SM_SHOTGUN.Succeeded())
		{
			shotgunMeshComponent->SetStaticMesh(SM_SHOTGUN.Object);
			shotgunMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("NoCollision"));

			FAttachmentTransformRules atr = FAttachmentTransformRules(
				EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
			shotgunMeshComponent->SetupAttachment(GetMesh(), TEXT("ShotgunSocket"));
			//shotgunMeshComponent->AttachToComponent(GetMesh(), atr, TEXT("ShotgunSocket"));

			shotgunMeshComponent->SetVisibility(false);

			for (int i = 0; i < 8; ++i)
			{
				snowballBombDirArray.Add(FVector());
			}
		}
	}

	if (!umbrella1MeshComponent)
	{
		umbrella1MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("umbrella1MeshComponent"));
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_UMBRELLA1(TEXT("/Game/NonCharacters/umbrella1.umbrella1"));
		if (SK_UMBRELLA1.Succeeded())
		{
			umbrella1MeshComponent->SetSkeletalMesh(SK_UMBRELLA1.Object);
			umbrella1MeshComponent->BodyInstance.SetCollisionProfileName(TEXT("NoCollision"));
			umbrella1MeshComponent->SetupAttachment(GetMesh(), TEXT("UmbrellaSocket"));
			umbrella1MeshComponent->SetVisibility(false);
		}
	}

	if (!umbrella2MeshComponent)
	{
		umbrella2MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("umbrella2MeshComponent"));
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_UMBRELLA2(TEXT("/Game/NonCharacters/umbrella2.umbrella2"));
		if (SK_UMBRELLA2.Succeeded())
		{
			umbrella2MeshComponent->SetSkeletalMesh(SK_UMBRELLA2.Object);
			umbrella2MeshComponent->BodyInstance.SetCollisionProfileName(TEXT("NoCollision"));
			umbrella2MeshComponent->SetupAttachment(GetMesh(), TEXT("UmbrellaSocket"));
			umbrella2MeshComponent->SetVisibility(false);
		}
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> UMB1_ANIM(TEXT("/Game/Animations/Umbrella/Umb1AnimBP.Umb1AnimBP_C"));
	if (UMB1_ANIM.Succeeded())
	{
		umbrella1Anim = UMB1_ANIM.Class;
		umbrella1MeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		umbrella1MeshComponent->SetAnimInstanceClass(UMB1_ANIM.Class);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> UMB2_ANIM(TEXT("/Game/Animations/Umbrella/Umb2AnimBP.Umb2AnimBP_C"));
	if (UMB2_ANIM.Succeeded())
	{
		umbrella2Anim = UMB2_ANIM.Class;
		umbrella2MeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		umbrella2MeshComponent->SetAnimInstanceClass(UMB2_ANIM.Class);
	}

	if (!umb1CollisionComponent)
	{
		umb1CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("umb1CollisionComponent"));
		umb1CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("BlockAll"));
		umb1CollisionComponent->SetBoxExtent(FVector(4.0f, 4.0f, 39.0f));
		umb1CollisionComponent->SetupAttachment(umbrella1MeshComponent);
		umb1CollisionComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 44.0f));

		umb1CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		umb1CollisionComponent->BodyInstance.bNotifyRigidBodyCollision = true;
		umb1CollisionComponent->SetUseCCD(true);
	}

	if (!umb2CollisionComponent)
	{
		umb2CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("umb2CollisionComponent"));
		umb2CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("BlockAll"));
		umb2CollisionComponent->SetBoxExtent(FVector(55.0f, 55.0f, 9.0f));
		umb2CollisionComponent->SetupAttachment(umbrella2MeshComponent);
		umb2CollisionComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 68.0f));

		umb2CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		umb2CollisionComponent->BodyInstance.bNotifyRigidBodyCollision = true;
		umb2CollisionComponent->SetUseCCD(true);
	}

	if (!bagMeshComponent)
	{
		bagMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bagMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BAG(TEXT("/Game/NonCharacters/bag.bag"));
		if (SM_BAG.Succeeded())
		{
			bagMeshComponent->SetStaticMesh(SM_BAG.Object);
			bagMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("NoCollision"));

			bagMeshComponent->SetupAttachment(GetMesh(), TEXT("BagSocket"));

			bagMeshComponent->SetVisibility(false);
		}
	}

	if (!projectilePath)
	{
		projectilePath = CreateDefaultSubobject<USplineComponent>(TEXT("ProjectilePath"));
		projectilePath->SetupAttachment(GetMesh());
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Spline(TEXT("/Game/NonCharacters/Spline_SM.Spline_SM"));
	if (SM_Spline.Succeeded())
	{
		for (int i = 0; i < iNumOfPathSpline; ++i)
		{
			USplineMeshComponent* splineMesh = CreateDefaultSubobject<USplineMeshComponent>(*(SplineStringArray[i]));
			splineMesh->SetStaticMesh(SM_Spline.Object);
			splineMesh->SetVisibility(false);
			//splineMesh->bOnlyOwnerSee = true;	// 해당 플레이어만 궤적이 보이도록
			splineMesh->SetCastShadow(false);	// 궤적의 그림자 안보이도록
			splineMeshComponents.Add(splineMesh);
		}
	}

	if (!projectilePathStartPos)
	{
		projectilePathStartPos = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectilePathStartPos"));
		projectilePathStartPos->SetupAttachment(GetMesh());
		projectilePathStartPos->SetRelativeLocation(FVector(-32.0f, 38.012852f, 116.264641f));
	}

	if (!jetskiMeshComponent)
	{
		jetskiMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("jetskiMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Jetski(TEXT("/Game/NonCharacters/SM_Jetski.SM_Jetski"));
		if (SM_Jetski.Succeeded())
		{
			jetskiMeshComponent->SetStaticMesh(SM_Jetski.Object);
			jetskiMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("Jetski"));
			jetskiMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			jetskiMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -56.0f));
			jetskiMeshComponent->SetVisibility(false);
		}
	}

	if (!driveAnimAsset)
	{
		static ConstructorHelpers::FObjectFinder<UAnimationAsset> ANIM_Drive(TEXT("/Game/Animations/Bear/bear_driving_Anim.bear_driving_Anim"));
		if (ANIM_Drive.Succeeded())
		{
			driveAnimAsset = ANIM_Drive.Object;
		}
	}

	SettingHead();

	GetCharacterMovement()->JumpZVelocity = 800.0f;
	isAttacking = false;

	projectileClass = AMySnowball::StaticClass();
	shotgunProjectileClass = ASnowballBomb::StaticClass();
	iceballClass = AIceball::StaticClass();
	jetskiClass = AJetski::StaticClass();

	iSessionId = -1;
	iCurrentHP = iMaxHP;	// 실제 설정값
	//iCurrentHP = iMinHP + 1;	// 디버깅용 - 대기시간 후 눈사람으로 변화

	snowball = nullptr;
	iceball = nullptr;
	
	iMaxSnowballCount = iOriginMaxSnowballCount;
	iCurrentSnowballCount = 0;	// 실제 설정값
	//iCurrentSnowballCount = 10;	// 디버깅용
	iMaxIceballCount = iOriginMaxIceballCount;
	iCurrentIceballCount = 0;	// 실제 설정값
	//iCurrentIceballCount = 10;	// 디버깅용
	iMaxMatchCount = iOriginMaxMatchCount;
	iCurrentMatchCount = 0;
	bHasUmbrella = false;
	bHasBag = false;
	bHasShotgun = false;

	farmingItem = nullptr;
	bIsFarming = false;
	
	bIsInsideOfBonfire = false;

	iCharacterState = CharacterState::AnimalNormal;
	bIsSnowman = false;
	GetCharacterMovement()->MaxWalkSpeed = iNormalSpeed;	// 캐릭터 이동속도 설정
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->PushForceFactor = 75000.0f;		// 기본값 750000.0f
	GetCharacterMovement()->MaxAcceleration = 1024.0f;		// 기본값 2048.0f

	iSelectedItem = ItemTypeList::Match;	// 선택된 아이템 기본값 - 성냥
	
	bIsInTornado = false;
	rotateCont = false;

	iSelectedWeapon = Weapon::Hand;
	iSelectedProjectile = Projectile::Snowball;

	iUmbrellaState = UmbrellaState::UmbClosed;
	bReleaseUmbrella = true;

	smokeNiagara = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/FX/NS_Smoke.NS_Smoke"), nullptr, LOAD_None, nullptr);

	overlappedTornado = nullptr;

	bIsAiming = false;
	fAimingElapsedTime = 0.0f;

	bIsRiding = false;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 수정 필요 - 캐릭터의 session id가 결정될 때 이 함수가 호출되도록
	SetCharacterMaterial(iSessionId-1);	// 캐릭터 머티리얼 설정(색상)

	playerController = Cast<APlayerController>(GetController());	// 생성자에서 하면 x (컨트롤러가 생성되기 전인듯)
	localPlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	
	SetAimingCameraPos();

	WaitForStartGame();	// 대기시간

	camera3->Deactivate();
}

void AMyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	MYLOG(Warning, TEXT("endplay"));
	//AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	//PlayerController->GetSocket()->StopListen();

	if (iSessionId == localPlayerController->iSessionId)
	{
		localPlayerController->mySocket->Send_LogoutPacket(iSessionId);
		SleepEx(0, true);
		localPlayerController->mySocket->CloseSocket();
		localPlayerController->mySocket->~ClientSocket();
		//delete[] localPlayerController->mySocket;
	}
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
		UpdateAiming();
		UpdateJetski();
	}

	UpdateZByTornado();		// 캐릭터가 토네이도 내부인 경우 z값 증가
	UpdateControllerRotateByTornado();	// 토네이도로 인한 스턴상태인 경우 회전하도록

	if (bHeadAnimEnd) 
	{
		GetWorld()->GetTimerManager().ClearTimer(HeadHandle);
	}
}

void AMyCharacter::init_Socket()
{
	/*AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerController->mySocket = nullptr;
	PlayerController->mySocket = new ClientSocket();
	PlayerController->mySocket->SetPlayerController(PlayerController);
	g_socket = PlayerController->mySocket;
	PlayerController->mySocket->Connect();
	
	DWORD recv_flag = 0;
	ZeroMemory(&g_socket->_recv_over._wsa_over, sizeof(g_socket->_recv_over._wsa_over));
	g_socket->_recv_over._wsa_buf.buf = reinterpret_cast<char*>(g_socket->_recv_over._net_buf + g_socket->_prev_size);
	g_socket->_recv_over._wsa_buf.len = sizeof(g_socket->_recv_over._net_buf) - g_socket->_prev_size;
	WSARecv(g_socket->_socket, &g_socket->_recv_over._wsa_buf, 1, 0, &recv_flag, &g_socket->_recv_over._wsa_over, recv_callback);
	g_socket->Send_LoginPacket();
	SleepEx(0, true);*/
}

void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	myAnim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	MYCHECK(nullptr != myAnim);

	myAnim->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnAttackMontageEnded);

	umb1AnimInst = Cast<UUmbrellaAnimInstance>(umbrella1MeshComponent->GetAnimInstance());
	umb2AnimInst = Cast<UUmbrellaAnimInstance>(umbrella2MeshComponent->GetAnimInstance());
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
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Released, this, &AMyCharacter::ReleaseAttack);
	PlayerInputComponent->BindAction(TEXT("Farming"), EInputEvent::IE_Pressed, this, &AMyCharacter::StartFarming);
	PlayerInputComponent->BindAction(TEXT("Farming"), EInputEvent::IE_Released, this, &AMyCharacter::EndFarming);

	PlayerInputComponent->BindAction(TEXT("SelectMatch"), EInputEvent::IE_Pressed, this, &AMyCharacter::SelectMatch);
	PlayerInputComponent->BindAction(TEXT("SelectUmbrella"), EInputEvent::IE_Pressed, this, &AMyCharacter::SelectUmbrella);
	PlayerInputComponent->BindAction(TEXT("UseSelectedItem"), EInputEvent::IE_Pressed, this, &AMyCharacter::UseSelectedItem);
	PlayerInputComponent->BindAction(TEXT("UseSelectedItem"), EInputEvent::IE_Released, this, &AMyCharacter::ReleaseRightMouseButton);

	PlayerInputComponent->BindAction(TEXT("ChangeWeapon"), EInputEvent::IE_Pressed, this, &AMyCharacter::ChangeWeapon);
	PlayerInputComponent->BindAction(TEXT("ChangeProjectile"), EInputEvent::IE_Pressed, this, &AMyCharacter::ChangeProjectile);

	PlayerInputComponent->BindAction(TEXT("GetOnOffJetski"), EInputEvent::IE_Pressed, this, &AMyCharacter::GetOnOffJetski);

	// Cheat Key
	PlayerInputComponent->BindAction(TEXT("Cheat_Teleport1"), EInputEvent::IE_Pressed, this, &AMyCharacter::Cheat_Teleport1);
	PlayerInputComponent->BindAction(TEXT("Cheat_Teleport2"), EInputEvent::IE_Pressed, this, &AMyCharacter::Cheat_Teleport2);
	PlayerInputComponent->BindAction(TEXT("Cheat_Teleport3"), EInputEvent::IE_Pressed, this, &AMyCharacter::Cheat_Teleport3);
	PlayerInputComponent->BindAction(TEXT("Cheat_Teleport4"), EInputEvent::IE_Pressed, this, &AMyCharacter::Cheat_Teleport4);

	PlayerInputComponent->BindAction(TEXT("Cheat_IncreaseHP"), EInputEvent::IE_Pressed, this, &AMyCharacter::Cheat_IncreaseHP);
	PlayerInputComponent->BindAction(TEXT("Cheat_DecreaseHP"), EInputEvent::IE_Pressed, this, &AMyCharacter::Cheat_DecreaseHP);

	PlayerInputComponent->BindAction(TEXT("Cheat_IncreaseSnowball"), EInputEvent::IE_Pressed, this, &AMyCharacter::Cheat_IncreaseSnowball);
}

void AMyCharacter::UpDown(float NewAxisValue)
{
	AddMovementInput(GetActorForwardVector(), NewAxisValue);
}

void AMyCharacter::LeftRight(float NewAxisValue)
{
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
	if (bIsSnowman) return;

	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	if (iSessionId == PlayerController->iSessionId)
	{
		switch (iSelectedProjectile) {
		case Projectile::Snowball:
			switch (iSelectedWeapon) {
			case Weapon::Hand:
				if (iCurrentSnowballCount <= 0) return;	// 눈덩이를 소유하고 있지 않으면 공격 x
				PlayerController->SendPlayerInfo(COMMAND_SNOWBALL);
				isAttacking = true;
				break;
			case Weapon::Shotgun:
				if (iCurrentSnowballCount < 5) return;	// 눈덩이가 5개 이상 없으면 공격 x
				PlayerController->SendPlayerInfo(COMMAND_SHOTGUN);
				MYLOG(Warning, TEXT("gunattack"));
				//ShotgunAttack();
				isAttacking = true;
				break;
			default:
				break;
			}
			break;
		case Projectile::Iceball:
			if (iCurrentIceballCount <= 0) return;	// 아이스볼을 소유하고 있지 않으면 공격 x
			PlayerController->SendPlayerInfo(COMMAND_ICEBALL);
			//IceballAttack();
			isAttacking = true;
			break;
		default:
			break;
		}
	}
#ifdef SINGLEPLAY_DEBUG
	SnowBallAttack();
#endif
}

void AMyCharacter::ReleaseAttack()
{	// 임시 - 아이스볼로 공격 중 release
	if (iSelectedProjectile == Projectile::Iceball)
	{
		if (iSessionId == localPlayerController->iSessionId)
		{
			SendCancelAttack(BULLET_ICEBALL);
		}
	}
	else
	{
		if (iSessionId == localPlayerController->iSessionId)
		{
			SendCancelAttack(BULLET_SNOWBALL);
		}
	}
}

//자기 자신을 포함하여 서버에서 명령이 오면 작업을 수행하게함
void AMyCharacter::Cancel_SnowBallAttack()
{
	if (myAnim->bThrowing)
	{
		myAnim->PlayAttack2MontageSectionEnd();
	}
	else
	{	// 눈덩이를 던지려다가 마우스 버튼을 릴리즈해서 취소된 경우
		StopAnimMontage();
		if (snowball)
		{
			snowball->Destroy();
			snowball = nullptr;
		}
	}

	if (iSessionId == localPlayerController->iSessionId)
	{

		localPlayerController->SetViewTargetWithBlend(this, fAimingTime);	// 기존 카메라로 전환
		localPlayerController->GetHUD()->bShowHUD = true;	// 크로스헤어 보이도록
	}
}

//자기 자신을 포함하여 서버에서 명령이 오면 작업을 수행하게함
void AMyCharacter::Cancel_IceBallAttack()
{
	//if (iSelectedProjectile == Projectile::Iceball)
	//{
		if (myAnim->bThrowing)
		{
			myAnim->PlayAttack2MontageSectionEnd();
		}
		else
		{	// 눈덩이를 던지려다가 마우스 버튼을 릴리즈해서 취소된 경우
			StopAnimMontage();
			if (iceball)
			{
				iceball->Destroy();
				iceball = nullptr;
			}
		}

		if (iSessionId == localPlayerController->iSessionId)
		{

			localPlayerController->SetViewTargetWithBlend(this, fAimingTime);	// 기존 카메라로 전환
			localPlayerController->GetHUD()->bShowHUD = true;	// 크로스헤어 보이도록
		}
	//}
}

void AMyCharacter::SnowBallAttack()
{
	//if (bIsSnowman) return;
	//if (iCurrentSnowballCount <= 0) return;	// 눈덩이를 소유하고 있지 않으면 공격 x

	//myAnim->PlayAttackMontage();
	myAnim->PlayAttack2Montage();

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

	if (iSessionId == localPlayerController->iSessionId)
	{
		localPlayerController->SetViewTargetWithBlend(aimingCameraPos, fAimingTime);	// 조준 시 카메라 위치로 전환
		localPlayerController->GetHUD()->bShowHUD = false;	// 크로스헤어 안보이도록
		bIsAiming = true;
	}
}

void AMyCharacter::ShotgunAttack()
{
	MYLOG(Warning, TEXT("AttackShotGun"));

	myAnim->PlayAttackShotgunMontage();

	// 디버깅용 - 실제는 주석 해제
	//iCurrentSnowballCount -= 4;	// 공격 시 눈덩이 소유량 4 감소
	UpdateUI(UICategory::CurSnowball);

}

void AMyCharacter::IceballAttack()
{
	//if (bIsSnowman) return;
	//if (iCurrentIceballCount <= 0) return;	// 아이스볼을 소유하고 있지 않으면 공격 x

	//myAnim->PlayAttackMontage();
	myAnim->PlayAttack2Montage();

	// Attempt to fire a projectile.
	if (iceballClass)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			FTransform SnowballSocketTransform = GetMesh()->GetSocketTransform(TEXT("SnowballSocket"));
			iceball = World->SpawnActor<AIceball>(iceballClass, SnowballSocketTransform, SpawnParams);
			FAttachmentTransformRules atr = FAttachmentTransformRules(
				EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
			iceball->AttachToComponent(GetMesh(), atr, TEXT("SnowballSocket"));
		}
	}

	if (iSessionId == localPlayerController->iSessionId)
	{
		localPlayerController->SetViewTargetWithBlend(aimingCameraPos, fAimingTime);	// 조준 시 카메라 위치로 전환
		localPlayerController->GetHUD()->bShowHUD = false;	// 크로스헤어 안보이도록
		bIsAiming = true;
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
		if (iSessionId == localPlayerController->iSessionId)
		{
			localPlayerController->SendPlayerInfo(COMMAND_DAMAGE);
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

//발사
void AMyCharacter::SendReleaseBullet(int bullet)
{
	if (iSessionId != localPlayerController->iSessionId) return;

	switch (bullet)
	{
	case BULLET_SNOWBALL: {
		if (IsValid(snowball))
		{
			
			localPlayerController->SendPlayerInfo(COMMAND_THROW_SB);
		}
		break;
	}
	case BULLET_ICEBALL: {
		if (IsValid(iceball))
		{
			localPlayerController->SendPlayerInfo(COMMAND_THROW_IB);
		}
		break;
	}
	default:
		break;
	}
}

//공격 취소
void AMyCharacter::SendCancelAttack(int bullet)
{
	if (iSessionId != localPlayerController->iSessionId) return;
	switch (bullet)
	{
	case BULLET_SNOWBALL: {
		if (IsValid(snowball))
		{
			localPlayerController->SendPlayerInfo(COMMAND_CANCEL_SB);

		}
		break;
	}
	case BULLET_ICEBALL: {
		if (IsValid(iceball))
		{
			localPlayerController->SendPlayerInfo(COMMAND_CANCEL_IB);

		}
		break;
	}
	default:
		break;
	}

}


void AMyCharacter::SendSpawnSnowballBomb()
{
	if (iSessionId != localPlayerController->iSessionId) return;
	if (shotgunProjectileClass)
	{
		MYLOG(Warning, TEXT("SendSpawnSnowballBomb"));
		localPlayerController->SendPlayerInfo(COMMAND_GUNFIRE);

	}
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

			float speed = PlayerController->GetCharactersInfo()->players[iSessionId].fSpeed;

			II_Throwable::Execute_Throw(snowball, direction_, speed);
			// 속도 인자 추가
#endif
#ifdef SINGLEPLAY_DEBUG
			FVector cameraLocation;
			FRotator cameraRotation;
			GetActorEyesViewPoint(cameraLocation, cameraRotation);
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());

			II_Throwable::Execute_Throw(snowball, cameraRotation.Vector());
#endif
			snowball = nullptr;

			bIsAiming = false;
			fAimingElapsedTime = 0.0f;
		}

	}
}

void AMyCharacter::ReleaseIceball()
{
	if (IsValid(iceball))
	{
		//iCurrentIceballCount -= 1;	// 공격 시 아이스볼 소유량 1 감소
		//UpdateUI(UICategory::CurIceball);

		FDetachmentTransformRules dtr = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
		iceball->DetachFromActor(dtr);

		if (iceball->GetClass()->ImplementsInterface(UI_Throwable::StaticClass()))
		{

#ifdef MULTIPLAY_DEBUG
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
			FVector direction_;
			direction_.X = PlayerController->GetCharactersInfo()->players[iSessionId].fCDx;
			direction_.Y = PlayerController->GetCharactersInfo()->players[iSessionId].fCDy;
			direction_.Z = PlayerController->GetCharactersInfo()->players[iSessionId].fCDz;
			
			FVector cameraLocation;
			FRotator cameraRotation;
			GetActorEyesViewPoint(cameraLocation, cameraRotation);

			cameraRotation.Vector() = direction_;

			float speed = PlayerController->GetCharactersInfo()->players[iSessionId].fSpeed;

			II_Throwable::Execute_IceballThrow(iceball, cameraRotation, speed);

			iceball = nullptr;

			bIsAiming = false;
			fAimingElapsedTime = 0.0f;

#endif
#ifdef SINGLEPLAY_DEBUG
			FVector cameraLocation;
			FRotator cameraRotation;
			GetActorEyesViewPoint(cameraLocation, cameraRotation);
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());

			float speed = fSnowballInitialSpeed + fAimingElapsedTime * fThrowPower;

			II_Throwable::Execute_IceballThrow(iceball, cameraRotation, speed);

			iceball = nullptr;

			bIsAiming = false;
			fAimingElapsedTime = 0.0f;
#endif
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
			//PlayerController->SetCharacterState(iSessionId, ST_ANIMAL);
			//PlayerController->SetCharacterHP(iSessionId, iMaxHP);
			//ChangeAnimal();
			PlayerController->GetSocket()->Send_StatusPacket(ST_ANIMAL, iSessionId);
			//UpdateTemperatureState();
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
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	if (iSessionId != PlayerController->iSessionId || !PlayerController->is_start()) return;
	if (Cast<ASnowdrift>(farmingItem))
	{
		if (iCurrentSnowballCount >= iMaxSnowballCount) return;	// 눈덩이 최대보유량 이상은 눈 무더기 파밍 못하도록
		bIsFarming = true;
		UpdateUI(UICategory::IsFarmingSnowdrift);
	}
	else if (Cast<AIcedrift>(farmingItem))
	{
		if (iCurrentIceballCount >= iMaxIceballCount) return;	// 아이스볼 최대보유량 이상은 눈 무더기 파밍 못하도록
		bIsFarming = true;
		UpdateUI(UICategory::IsFarmingSnowdrift);
	}
	else if (Cast<AItembox>(farmingItem))
	{
		AItembox* itembox = Cast<AItembox>(farmingItem);
		switch (itembox->GetItemboxState())
		{
		case ItemboxState::Closed:
			itembox->SetItemboxState(ItemboxState::Opening);
			PlayerController->GetSocket()->Send_OpenBoxPacket(itembox->GetId());
			break;
		case ItemboxState::Opened:
			// 아이템박스에서 내용물 파밍에 성공하면 아이템박스에서 아이템 제거 (박스는 그대로 유지시킴)
			if (GetItem(itembox->GetItemType())) { 
				MYLOG(Warning, TEXT("item %d"), itembox->GetItemType());

				MYLOG(Warning, TEXT("item %d"), itembox->GetItemType());
				//아이템 파밍 시 서버 전송
#ifdef MULTIPLAY_DEBUG
				PlayerController->GetSocket()->Send_ItemPacket(itembox->GetItemType(), itembox->GetId());
#endif

				//itembox->DeleteItem(); //서버에서 패킷받았을 때 처리
				//박스가 열리는 시점에서 아이템 동기화
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
		GetBag();
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
			UpdateUI(UICategory::IsFarmingSnowdrift);
		}
		else if (Cast<AIcedrift>(farmingItem))
		{

			if (iCurrentIceballCount >= iMaxIceballCount) return;

			// F키로 얼음 무더기 파밍 중 F키 release 시 얼음 무더기 duration 초기화
			AIcedrift* icedrift = Cast<AIcedrift>(farmingItem);
			icedrift->SetFarmDuration(AIcedrift::fFarmDurationMax);
			bIsFarming = false;
			UpdateUI(UICategory::IsFarmingSnowdrift);
		}
	}
}

void AMyCharacter::UpdateFarming(float deltaTime)
{
	if (!bIsFarming) return;
	if (!IsValid(farmingItem)) return;
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	if (iSessionId != PlayerController->iSessionId || !PlayerController->is_start()) return;

	// 눈 무더기 파밍
	ASnowdrift* snowdrift = Cast<ASnowdrift>(farmingItem);
	if (snowdrift)
	{	// 눈 무더기 duration 만큼 F키를 누르고 있으면 캐릭터의 눈덩이 추가 
		float lastFarmDuration = snowdrift->GetFarmDuration();
		float newFarmDuration = lastFarmDuration - deltaTime;
		snowdrift->SetFarmDuration(newFarmDuration);
		UpdateUI(UICategory::SnowdriftFarmDuration, newFarmDuration);

		if (newFarmDuration <= 0)
		{
			//눈덩이 파밍 시 서버 전송
#ifdef MULTIPLAY_DEBUG
			PlayerController->GetSocket()->Send_ItemPacket(ITEM_SNOW, snowdrift->GetId());
#endif
			UpdateUI(UICategory::CurSnowball);
			bIsFarming = false;	// 눈무더기 파밍 끝나면 false로 변경 후 UI 갱신 (눈무더기 파밍 바 ui 안보이도록)
			UpdateUI(UICategory::IsFarmingSnowdrift);
			//snowdrift->Destroy(); //서버에서 아이디 반환시 처리
			//snowdrift = nullptr;
		}
	}

	// 얼음 무더기 파밍
	AIcedrift* icedrift = Cast<AIcedrift>(farmingItem);
	if (icedrift)
	{	// 얼음 무더기 duration 만큼 F키를 누르고 있으면 캐릭터의 아이스볼 추가 
		float lastFarmDuration = icedrift->GetFarmDuration();
		float newFarmDuration = lastFarmDuration - deltaTime;
		icedrift->SetFarmDuration(newFarmDuration);
		UpdateUI(UICategory::SnowdriftFarmDuration, newFarmDuration);

		if (newFarmDuration <= 0)
		{
			iCurrentIceballCount = FMath::Clamp<int>(iCurrentIceballCount + icedrift->iNumOfIceball, 0, iMaxIceballCount);
			UpdateUI(UICategory::CurIceball);
			bIsFarming = false;	// 얼음무더기 파밍 끝나면 false로 변경 후 UI 갱신 (눈무더기 파밍 바 ui 안보이도록)
			UpdateUI(UICategory::IsFarmingSnowdrift);
			icedrift->Destroy();
			icedrift = nullptr;
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
	if (bIsRiding) return;	// jetski 탑승 중에는 체온에따른 speed x

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
	ResetHasItems();
	UpdateUI(UICategory::AllOfUI);
	

#ifdef SINGLEPLAY_DEBUG
	UpdateTemperatureState();
	UpdateUI(UICategory::HP);	// 변경된 체력으로 ui 갱신
#endif

	GetCharacterMovement()->MaxWalkSpeed = iSlowSpeed;	// 눈사람의 이동속도는 슬로우 상태인 캐릭터와 동일하게 설정
	
	StartStun(fChangeSnowmanStunTime);

	isAttacking = false;	// 공격 도중에 상태 변할 시 발생하는 오류 방지

	CloseUmbrellaAnim();
	HideUmbrella();
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
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	if (iSessionId != PlayerController->iSessionId || !PlayerController->is_start()) return;
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
			if (iSessionId == PlayerController->iSessionId && PlayerController->is_start())
				PlayerController->GetSocket()->Send_StatusPacket(ST_OUTBURN, iSessionId);
		}
	//}
}

void AMyCharacter::StartStun(float waitTime)
{
	//if (!playerController) return;	// 다른 플레이어의 캐릭터는 플레이어 컨트롤러가 존재 x?

	if (iCharacterState == CharacterState::SnowmanStunned)
	{	// 스턴 상태에서 또 맞으면 기존에 실행중이던 stunhandle 초기화 (핸들러 새로 갱신하도록)
		GetWorldTimerManager().ClearTimer(stunHandle);
	}

	// 플레이어의 입력을 무시하도록 (움직일 수 없음, 시야도 고정, 상태 - Stunned)
	iCharacterState = bIsSnowman ? CharacterState::SnowmanStunned : CharacterState::AnimalStunned;
	DisableInput(playerController);
	GetMesh()->bPauseAnims = true;	// 캐릭터 애니메이션도 정지

	EndStun(waitTime);	// waitTime이 지나면 stun이 끝나도록

	if (bIsInTornado)
	{	// 토네이도에 의한 스턴이면 캐릭터가 회전하도록, 애니메이션은 재생되도록
		rotateCont = true;
		GetMesh()->bPauseAnims = false;
	}
}

void AMyCharacter::EndStun(float waitTime)
{
	//if (!playerController) return;

	// x초 후에 다시 입력을 받을 수 있도록 (움직임과 시야 제한 해제, 상태 - Snowman)
	GetWorld()->GetTimerManager().SetTimer(stunHandle, FTimerDelegate::CreateLambda([&]()
		{
			iCharacterState = bIsSnowman ? CharacterState::SnowmanNormal : CharacterState::AnimalNormal;
			EnableInput(playerController);
			GetMesh()->bPauseAnims = false;

			rotateCont = false;		// 토네이도에 의해서 회전 중이면 멈추기
		}), waitTime, false);
}

void AMyCharacter::ResetHasItems()
{
	iCurrentSnowballCount = 0;
	iMaxSnowballCount = iOriginMaxSnowballCount;
	iCurrentIceballCount = 0;
	iMaxIceballCount = iOriginMaxIceballCount;
	iCurrentMatchCount = 0;
	iMaxMatchCount = iOriginMaxMatchCount;
	bHasUmbrella = false;
	bHasBag = false;
	bHasShotgun = false;
	bagMeshComponent->SetVisibility(false);


	UpdateUI(UICategory::AllOfUI);
}

void AMyCharacter::ChangeAnimal()
{
	bIsSnowman = false;

	// 스켈레탈메시, 애니메이션 블루프린트 변경
	GetMesh()->SetSkeletalMesh(bear);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetAnimInstanceClass(bearAnim);
	SetCharacterMaterial(iSessionId-1);

	myAnim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	MYCHECK(nullptr != myAnim);
	myAnim->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnAttackMontageEnded);
	
	iCurrentHP = iBeginSlowHP;	// 눈사람 -> 동물 부활 시 체력 동상 상태(슬로우)로 설정 (30.0 - 체력의 1/4)
	GetWorldTimerManager().ClearTimer(temperatureHandle);
	ResetHasItems();
	UpdateUI(UICategory::AllOfUI);
	UpdateTemperatureState();
	
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

void AMyCharacter::UpdateUI(int uiCategory, float farmDuration)
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
	case UICategory::CurIceball:
		localPlayerController->CallDelegateUpdateCurrentIceballCount();		// CurIceball ui 갱신
		break;
	case UICategory::CurMatch:
		localPlayerController->CallDelegateUpdateCurrentMatchCount();	// CurMatch ui 갱신
		break;
	case UICategory::MaxSnowIceballAndMatch:
		localPlayerController->CallDelegateUpdateMaxSnowIceballAndMatchCount();	// MaxSnowIceballAndMatch ui 갱신
		break;
	case UICategory::HasUmbrella:
		localPlayerController->CallDelegateUpdateHasUmbrella();	// HasUmbrella ui 갱신
		break;
	case UICategory::HasBag:
		localPlayerController->CallDelegateUpdateHasBag();	// HasBag ui 갱신
		break;
	case UICategory::HasShotgun:
		localPlayerController->CallDelegateUpdateHasShotgun();	// HasShotgun ui 갱신
		break;
	case UICategory::IsFarmingSnowdrift:
		localPlayerController->CallDelegateUpdateIsFarmingSnowdrift();	// snowdrift farming ui visible 갱신
		break;
	case UICategory::SnowdriftFarmDuration:
		localPlayerController->CallDelegateUpdateSnowdriftFarmDuration(farmDuration);	// snowdrift farm duration ui 갱신
		break;
	case UICategory::SelectedItem:
		localPlayerController->CallDelegateUpdateSelectedItem();
		break;
	case UICategory::SelectedProjectile:
		localPlayerController->CallDelegateUpdateSelectedProjectile();
		break;
	case UICategory::SelectedWeapon:
		localPlayerController->CallDelegateUpdateSelectedWeapon();
		break;
	case UICategory::AllOfUI:
		localPlayerController->CallDelegateUpdateAllOfUI();	// 모든 캐릭터 ui 갱신
		break;
	default:
		break;
	}
}

void AMyCharacter::SelectMatch()
{
	iSelectedItem = ItemTypeList::Match;
	UpdateUI(UICategory::SelectedItem);
}

void AMyCharacter::SelectUmbrella()
{
	iSelectedItem = ItemTypeList::Umbrella;
	UpdateUI(UICategory::SelectedItem);
}

void AMyCharacter::UseSelectedItem()
{
	if (bIsSnowman) return;	// 눈사람은 아이템 사용 x

	switch (iSelectedItem) {
	case ItemTypeList::Match: {	// 성냥 아이템 사용 시
		if (iCurrentMatchCount <= 0) return;	// 보유한 성냥이 없는 경우 리턴
		// 체력 += 20 (체온으로는 2.0도)
		if (iSessionId == localPlayerController->iSessionId)
		{
			localPlayerController->SendPlayerInfo(COMMAND_MATCH);
		}
		iCurrentMatchCount -= 1;
		UpdateUI(UICategory::CurMatch);
		break;
	}
	case ItemTypeList::Umbrella:	// 우산 아이템 사용 시
		if(!bHasUmbrella) break;   // 우산 아이템이 없는 경우 리턴
		if (iSessionId == localPlayerController->iSessionId)
		{
			MYLOG(Warning, TEXT("select_umb"));
			localPlayerController->SendPlayerInfo(COMMAND_UMB_START);
		}
		
		// 디버깅용 - 실제로는 주석 해제
		//StartUmbrella();
		break;
	default:
		break;
	}
}

void AMyCharacter::UpdateZByTornado()
{	// 캐릭터가 토네이도 내부인 경우 z값 증가
	if (bIsInTornado)
	{
		if (!(iSessionId == localPlayerController->iSessionId)) return;

		LaunchCharacter(FVector(0.0f, 0.0f, 20.0f), true, false);

		// 토네이도에 휩쓸린 캐릭터의 x, y값 토네이도의 x, y값으로 설정해서 같이 움직이도록
		if (overlappedTornado != nullptr)
		{
			SetActorLocation(FVector(overlappedTornado->GetActorLocation().X, overlappedTornado->GetActorLocation().Y, GetActorLocation().Z));
		}
	}
}

void AMyCharacter::UpdateControllerRotateByTornado()
{
	if (rotateCont)
	{
		if (!(iSessionId == localPlayerController->iSessionId)) return;

		FRotator contRot = localPlayerController->GetControlRotation();
		FRotator newContRot = FRotator(contRot.Pitch, contRot.Yaw + 5.0f, contRot.Roll);
		localPlayerController->SetControlRotation(newContRot);
	}
}



void AMyCharacter::ChangeWeapon()
{
	iSelectedWeapon = (iSelectedWeapon + 1) % iNumOfWeapons;
	UpdateUI(UICategory::SelectedWeapon);
}

void AMyCharacter::ChangeProjectile()
{
	iSelectedProjectile = (iSelectedProjectile + 1) % iNumOfProjectiles;
	UpdateUI(UICategory::SelectedProjectile);
}

void AMyCharacter::ShowShotgun()
{
	shotgunMeshComponent->SetVisibility(true);
}

void AMyCharacter::HideShotgun()
{
	shotgunMeshComponent->SetVisibility(false);
}

void AMyCharacter::SpawnSnowballBomb()
{
	if (shotgunProjectileClass)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			FVector cameraLocation;
			FRotator cameraRotation;

			GetActorEyesViewPoint(cameraLocation, cameraRotation);
			
		

			FVector rightVec1 = GetActorRightVector() / 15;
			FVector rightVec2 = GetActorRightVector() / 25;
			FVector rightVec3 = GetActorRightVector() / 35;

			float up1 = 0.055f;
			float up2 = 0.035f;
			float up3 = -0.025f;
			float up4 = -0.05f;
			
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
			
			cameraRotation.Pitch = PlayerController->GetCharactersInfo()->players[iSessionId].Pitch;

			snowballBombDirArray[0] = cameraRotation.Vector();
			snowballBombDirArray[1] = FVector(cameraRotation.Vector() + FVector(0.0f, 0.0f, up1));
			snowballBombDirArray[2] = FVector(cameraRotation.Vector() + FVector(0.0f, 0.0f, up2) + rightVec2);
			snowballBombDirArray[3] = FVector(cameraRotation.Vector() + FVector(0.0f, 0.0f, up2) - rightVec2);
			snowballBombDirArray[4] = FVector(cameraRotation.Vector() + FVector(0.0f, 0.0f, up3) + rightVec1);
			snowballBombDirArray[5] = FVector(cameraRotation.Vector() + FVector(0.0f, 0.0f, up3) - rightVec1);
			snowballBombDirArray[6] = FVector(cameraRotation.Vector() + FVector(0.0f, 0.0f, up4) + rightVec3);
			snowballBombDirArray[7] = FVector(cameraRotation.Vector() + FVector(0.0f, 0.0f, up4) - rightVec3);

			MYLOG(Warning, TEXT("pitch %f"), cameraRotation.Pitch);


			//TArray<int> randInt;
			//// 0~7 중에서 중복없이 5개의 숫자 설정
			//while (randInt.Num() < 5)
			//{
			//	int random = UKismetMathLibrary::RandomIntegerInRange(0, 7);
			//	if (randInt.Find(random) == -1)
			//	{
			//		randInt.Add(random);
			//	}
			//}

			// 랜덤한 5개의 위치에 snowball bomb 생성 및 던지기
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			FTransform muzzleSocketTransform = shotgunMeshComponent->GetSocketTransform(TEXT("Muzzle1Socket"));
			FTransform smokeSocketTransform = shotgunMeshComponent->GetSocketTransform(TEXT("SmokeSocket"));
			
			for (int i = 0; i < 5; ++i)
			{
				ASnowballBomb* snowballBomb = GetWorld()->SpawnActor<ASnowballBomb>(shotgunProjectileClass, muzzleSocketTransform, SpawnParams);

				II_Throwable::Execute_Throw(snowballBomb, snowballBombDirArray[PlayerController->GetCharactersInfo()->players[iSessionId].random_bullet[i]], 0.0f);
			}

			if (smokeNiagara) {
				UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), smokeNiagara, smokeSocketTransform.GetLocation(), FRotator(1), FVector(1), true, true, ENCPoolMethod::AutoRelease, true);
				//NiagaraComp->SetNiagaraVariableFloat(FString("StrengthCoef"), CoefStrength);
			}

			// 디버깅용 - muzzle socket 8곳에서 모두 발사
			//for (int i = 0; i < 8; ++i)
			//{
			//	ASnowballBomb* snowballBomb = GetWorld()->SpawnActor<ASnowballBomb>(shotgunProjectileClass, muzzleSocketTransform, SpawnParams);

			//	II_Throwable::Execute_Throw(snowballBomb, snowballBombDirArray[i]);
			//}
		}
	}
}

void AMyCharacter::Cheat_Teleport1()
{
	//모닥불
	if (iSessionId != localPlayerController->iSessionId) return;
	FVector CharacterLocation;
	CharacterLocation.X = -1441.876221;
	CharacterLocation.Y = 349.000549;
	CharacterLocation.Z = -2801.376953;
	SetActorLocation(CharacterLocation);
	//localPlayerController->SendTeleportInfo(TEL_FIRE);
}
void AMyCharacter::Cheat_Teleport2()
{
	//다리
	if (iSessionId != localPlayerController->iSessionId) return;
	FVector CharacterLocation;
	CharacterLocation.X = 1077.395142;
	CharacterLocation.Y = 4947.352051;
	CharacterLocation.Z = -3300.918213;
	SetActorLocation(CharacterLocation);
	//localPlayerController->SendTeleportInfo(TEL_BRIDGE);

}

void AMyCharacter::Cheat_Teleport3()
{
	//탑 위(섬 앞에 탑)
	if (iSessionId != localPlayerController->iSessionId) return;
	FVector CharacterLocation;
	CharacterLocation.X = -2666.638428;
	CharacterLocation.Y = 11660.651367;
	CharacterLocation.Z = 940.448914;
	SetActorLocation(CharacterLocation);
	//localPlayerController->SendTeleportInfo(TEL_TOWER);

}
void AMyCharacter::Cheat_Teleport4()
{
	//빙판
	if (iSessionId != localPlayerController->iSessionId) return;
	FVector CharacterLocation;
	CharacterLocation.X = -7328.154785;
	CharacterLocation.Y = -1974.412354;
	CharacterLocation.Z = -3816.000000;
	SetActorLocation(CharacterLocation);
	//localPlayerController->SendCheatInfo(TEL_ICE);

}
void AMyCharacter::Cheat_IncreaseHP()
{
	localPlayerController->SendCheatInfo(CHEAT_HP_UP);
}
void AMyCharacter::Cheat_DecreaseHP()
{
	localPlayerController->SendCheatInfo(CHEAT_HP_DOWN);

	// 임시 - 샷건 획득 치트키
	bHasShotgun = true;
	UpdateUI(UICategory::HasShotgun);
}
void AMyCharacter::Cheat_IncreaseSnowball()
{
	if (iSelectedProjectile == Projectile::Iceball)
	{
		localPlayerController->SendCheatInfo(CHEAT_ICE_PLUS);
	}
	else 
	{
		localPlayerController->SendCheatInfo(CHEAT_SNOW_PLUS);
	}
}

void AMyCharacter::ReleaseRightMouseButton()
{
	switch (iSelectedItem) {
	case ItemTypeList::Umbrella:	// 우산 사용 해제
		MYLOG(Warning, TEXT("relese_mouse"));
		ReleaseUmbrella();
		break;
	default:
		break;
	}
}

void AMyCharacter::StartUmbrella()
{
	if (isAttacking) return;

	IsAttacking = true;

	myAnim->PlayUmbrellaMontage();

	bReleaseUmbrella = false;
}

void AMyCharacter::ShowUmbrella()
{
	// 우산 메시 보이도록
	umbrella1MeshComponent->SetVisibility(true);
	umbrella2MeshComponent->SetVisibility(true);

	// 우산 메시 콜리전 활성화
	umb1CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	umb2CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	OpenUmbrellaAnim();		// 우산 펼치는 애니메이션
}

void AMyCharacter::HideUmbrella()
{
	// 우산 메시 안보이도록
	umbrella1MeshComponent->SetVisibility(false);
	umbrella2MeshComponent->SetVisibility(false);

	// 우산 메시 콜리전 비활성화
	umb1CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	umb2CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	umb1AnimInst->SetClose(false);
	umb2AnimInst->SetClose(false);

	iUmbrellaState = UmbrellaState::UmbClosed;
}

void AMyCharacter::OpenUmbrellaAnim()
{
	iUmbrellaState = UmbrellaState::UmbOpening;

	// 우산 메시 애니메이션 재생 (펼치는)
	umb1AnimInst->SetOpen(true);
	umb2AnimInst->SetOpen(true);
}

void AMyCharacter::CloseUmbrellaAnim()
{
	iUmbrellaState = UmbrellaState::UmbClosing;

	// 우산 메시 애니메이션 재생 (접히는)
	umb1AnimInst->SetClose(true);
	umb2AnimInst->SetClose(true);

	umb1AnimInst->SetOpen(false);
	umb2AnimInst->SetOpen(false);
}

void AMyCharacter::ReleaseUmbrella()
{
	bReleaseUmbrella = true;

	switch (iUmbrellaState) {
	case UmbrellaState::UmbClosed:
		break;
	case UmbrellaState::UmbOpening:
		break;
	case UmbrellaState::UmbOpened:
		if (iSessionId == localPlayerController->iSessionId)
		{
			MYLOG(Warning, TEXT("ReleaseUmbrella"));
			localPlayerController->SendPlayerInfo(COMMAND_UMB_END);
		}
		//myAnim->ResumeUmbrellaMontage();
		//CloseUmbrellaAnim();
		break;
	case UmbrellaState::UmbClosing:
		break;
	default:
		break;
	}
}

void AMyCharacter::GetBag()
{
	bHasBag = true;
	iMaxSnowballCount += 5;	// 눈덩이 10 -> 15 까지 보유 가능
	iMaxIceballCount += 5;	// 아이스볼 10 -> 15 까지 보유 가능
	iMaxMatchCount += 1;	// 성냥 2 -> 3 까지 보유 가능
	UpdateUI(UICategory::HasBag);
	UpdateUI(UICategory::MaxSnowIceballAndMatch);

	bagMeshComponent->SetVisibility(true);
}

void AMyCharacter::ShowProjectilePath()
{
	if (iSessionId != localPlayerController->iSessionId) return;	// 본인의 궤적만 그리도록

	projectilePath->ClearSplinePoints();
	HideProjectilePath();

	if (myAnim->bThrowing)
	{
		FHitResult OutHitResult;	// 사용 x
		TArray<FVector> OutPathPositions;
		FVector OutLastTraceDestination;	// 사용 x

		//FVector StartPos = GetMesh()->GetSocketLocation(TEXT("SnowballSocket"));
		FVector StartPos = projectilePathStartPos->GetComponentLocation();
		FVector cameraLocation;
		FRotator cameraRotation;
		GetActorEyesViewPoint(cameraLocation, cameraRotation);
		FVector LaunchVelocity = (cameraRotation.Vector() + FVector(0.0f, 0.0f, 0.15f)) * (2000.0f + fAimingElapsedTime * fThrowPower);
		//FVector LaunchVelocity = (cameraRotation.Vector() + FVector(0.0f, 0.0f, 0.15f)) * 2500.0f;
		// bool bTracePath, float ProjectileRadius, TEnumAsByte<ECollisionChannel> TraceChannel, bool bTraceComplex,
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(this);
		actorsToIgnore.Add(aimingCameraPos);
		//EDrawDebugTrace::Type DrawDebugType

		// float DrawDebugTime, float SimFrequency, float MaxSimTime, float OverrideGravityZ;

		UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel(GetWorld(), OutHitResult, OutPathPositions, OutLastTraceDestination,
			StartPos, LaunchVelocity, true, 0.0f, ECollisionChannel::ECC_Camera, false, actorsToIgnore, EDrawDebugTrace::None,
			0.0f, 15.0f, 2.0f, 0.0f);

		for (int i = 0; i < OutPathPositions.Num(); ++i)
		{
			projectilePath->AddSplinePointAtIndex(OutPathPositions[i], i, ESplineCoordinateSpace::Local);
		}

		int lastIndex = projectilePath->GetNumberOfSplinePoints() - 1;
		if (lastIndex > iNumOfPathSpline - 1) lastIndex = iNumOfPathSpline - 1;

		for (int i = 0; i < lastIndex; ++i)
		{
			FVector startPos, startTangent, endPos, endTangent;
			startPos = projectilePath->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
			startTangent = projectilePath->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
			endPos = projectilePath->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
			endTangent = projectilePath->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

			splineMeshComponents[i]->SetStartAndEnd(startPos, startTangent, endPos, endTangent);
			splineMeshComponents[i]->SetVisibility(true);
		}

		//Delay 함수
		FTimerHandle WaitHandle;
		float WaitTime = GetWorld()->GetDeltaSeconds();
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
			{
				ShowProjectilePath();
			}), WaitTime, false);
	}
}

void AMyCharacter::HideProjectilePath()
{
	for (int i = 0; i < iNumOfPathSpline; ++i)
	{
		splineMeshComponents[i]->SetVisibility(false);
	}
}

void AMyCharacter::SetAimingCameraPos()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	aimingCameraPos = GetWorld()->SpawnActor<AEmptyActor>(AEmptyActor::StaticClass(), GetActorTransform(), SpawnParams);
	FAttachmentTransformRules atr = FAttachmentTransformRules(
		EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
	aimingCameraPos->AttachToComponent(springArm2, atr);
}

void AMyCharacter::UpdateAiming()
{
	if (!bIsAiming) return;
	if (fAimingElapsedTime >= fMaxChargingTime) return;
	fAimingElapsedTime += GetWorld()->GetDeltaSeconds();
}

void AMyCharacter::GetOnOffJetski()
{
	if (bIsSnowman) return;		// 눈사람은 jetski 탑승 x

	if (iSessionId == localPlayerController->iSessionId)
	{
		//제트스키 탑승시 서버 전송
		localPlayerController->GetSocket()->Send_ItemPacket(ITEM_JET, 0);
	}

	if (!bIsRiding) GetOnJetski();
	else GetOffJetski();
}


void AMyCharacter::GetOnJetski()
{	// jetski 탑승

	TArray<AActor*> overlapActorsArray;	// overlap 중인 jetski를 담을 배열
	GetOverlappingActors(overlapActorsArray, jetskiClass);

	if (overlapActorsArray.Num() == 1)
	{	
		bIsRiding = true;

		// 애니메이션 BP 대신 drive 애니메이션 재생
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		GetMesh()->PlayAnimation(driveAnimAsset, true);
		
		// jetski 물리, 충돌, 시야 활성화
		jetskiMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		jetskiMeshComponent->SetSimulatePhysics(true);
		jetskiMeshComponent->SetVisibility(true);

		// 배치된 jetski의 위치로 캐릭터 이동 및 회전
		SetActorLocation(overlapActorsArray[0]->GetActorLocation());
		FRotator newRotation = FRotator(
			GetControlRotation().Pitch, overlapActorsArray[0]->GetActorRotation().Yaw, GetControlRotation().Roll);
		localPlayerController->SetControlRotation(newRotation);

		// 배치된 jetski 제거
		overlapActorsArray[0]->Destroy();

		// jetski 메시에 캐릭터 메시 attach
		FAttachmentTransformRules atr = FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
		GetMesh()->AttachToComponent(jetskiMeshComponent, atr);
		GetMesh()->SetRelativeLocation(FVector(-8.68f, -4.0f, 15.35f));

		// 운전용 카메라로 전환, 크로스헤어 제거
		camera->Deactivate();
		camera3->Activate();
		localPlayerController->GetHUD()->bShowHUD = false;

		// 이동속도 설정
		GetCharacterMovement()->MaxWalkSpeed = iJetskiSpeed;
	}
}

void AMyCharacter::GetOffJetski()
{	// jetski 하차

	bIsRiding = false;

	// jetski 왼쪽으로 캐릭터 이동 및 회전
	SetActorLocation(FVector(GetActorLocation() - jetskiMeshComponent->GetRightVector() * 80));
	localPlayerController->SetControlRotation(FRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f));

	// jetski 물리, 충돌, 시야 비활성화
	jetskiMeshComponent->SetSimulatePhysics(false);
	jetskiMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	jetskiMeshComponent->SetVisibility(false);

	// 캐릭터 메시 jetski 메시에서 detach
	GetMesh()->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	FAttachmentTransformRules atr = FAttachmentTransformRules(
		EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
	GetMesh()->AttachToComponent(GetCapsuleComponent(), atr);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -77.0f), FRotator(0.0f, -90.0f, 0.0f));

	// 해당 위치에 jetski 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	FVector location = FVector(jetskiMeshComponent->GetComponentLocation() + FVector(0.0f, 0.0f, 27.5f));
	FTransform transform = FTransform(jetskiMeshComponent->GetComponentRotation(), location, jetskiMeshComponent->GetRelativeScale3D());
	GetWorld()->SpawnActor<AJetski>(jetskiClass, transform, SpawnParams);

	// 기존 애니메이션 BP로 변경
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(bearAnim);
	myAnim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	MYCHECK(nullptr != myAnim);
	myAnim->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnAttackMontageEnded);

	// 기존 카메라로 전환, 크로스헤어 생성
	camera->Activate();
	camera3->Deactivate();
	localPlayerController->GetHUD()->bShowHUD = true;

	// 이동속도 설정
	GetCharacterMovement()->MaxWalkSpeed = iNormalSpeed;
}

void AMyCharacter::UpdateJetski()
{
	if (bIsRiding)
	{	// jetski 메시 위치 및 회전 갱신, 뒤집어짐 방지
		jetskiMeshComponent->SetWorldLocation(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 56.0f));
		float pitch = UKismetMathLibrary::ClampAngle(jetskiMeshComponent->GetComponentRotation().Pitch, -40.0f, 40.0f);
		float roll = UKismetMathLibrary::ClampAngle(jetskiMeshComponent->GetComponentRotation().Roll, -20.0f, 20.0f);
		//FRotator newRotation = FRotator(jetskiMeshComponent->GetComponentRotation().Pitch, GetActorRotation().Yaw, roll);
		FRotator newRotation = FRotator(pitch, GetActorRotation().Yaw, roll);
		jetskiMeshComponent->SetWorldRotation(newRotation);
	}
}

void AMyCharacter::SettingHead() 
{
	heads.Add(head1);
	heads.Add(head2);
	heads.Add(head3);
	heads.Add(head4);
	heads.Add(head5);
	heads.Add(head6);
	heads.Add(head7);
	heads.Add(head8);
	heads.Add(head9);
	heads.Add(head10);
	heads.Add(head11);
	heads.Add(head12);
	heads.Add(head13);
	heads.Add(head14);
	heads.Add(head15);
	heads.Add(head16);
	heads.Add(head17);
	heads.Add(head18);
	heads.Add(head19);
	heads.Add(head20);
	heads.Add(head21);
	heads.Add(head22);
	heads.Add(head23);

	TArray<const wchar_t*> names;
	names.Add(TEXT("head1"));
	names.Add(TEXT("head2"));
	names.Add(TEXT("head3"));
	names.Add(TEXT("head4"));
	names.Add(TEXT("head5"));
	names.Add(TEXT("head6"));
	names.Add(TEXT("head7"));
	names.Add(TEXT("head8"));
	names.Add(TEXT("head9"));
	names.Add(TEXT("head10"));
	names.Add(TEXT("head11"));
	names.Add(TEXT("head12"));
	names.Add(TEXT("head13"));
	names.Add(TEXT("head14"));
	names.Add(TEXT("head15"));
	names.Add(TEXT("head16"));
	names.Add(TEXT("head17"));
	names.Add(TEXT("head18"));
	names.Add(TEXT("head19"));
	names.Add(TEXT("head20"));
	names.Add(TEXT("head21"));
	names.Add(TEXT("head22"));
	names.Add(TEXT("head23"));

	static TArray<ConstructorHelpers::FObjectFinder<UStaticMesh>*> SM_HEADS;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD1(TEXT("/Game/FX/Frozen/Meshes/head_2.head_2"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD2(TEXT("/Game/FX/Frozen/Meshes/head_3.head_3"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD3(TEXT("/Game/FX/Frozen/Meshes/head_4.head_4"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD4(TEXT("/Game/FX/Frozen/Meshes/head_5.head_5"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD5(TEXT("/Game/FX/Frozen/Meshes/head_6.head_6"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD6(TEXT("/Game/FX/Frozen/Meshes/head_7.head_7"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD7(TEXT("/Game/FX/Frozen/Meshes/head_8.head_8"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD8(TEXT("/Game/FX/Frozen/Meshes/head_9.head_9"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD9(TEXT("/Game/FX/Frozen/Meshes/head_10.head_10"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD10(TEXT("/Game/FX/Frozen/Meshes/head_11.head_11"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD11(TEXT("/Game/FX/Frozen/Meshes/head_12.head_12"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD12(TEXT("/Game/FX/Frozen/Meshes/head_13.head_13"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD13(TEXT("/Game/FX/Frozen/Meshes/head_14.head_14"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD14(TEXT("/Game/FX/Frozen/Meshes/head_15.head_15"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD15(TEXT("/Game/FX/Frozen/Meshes/head_16.head_16"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD16(TEXT("/Game/FX/Frozen/Meshes/head_17.head_17"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD17(TEXT("/Game/FX/Frozen/Meshes/head_18.head_18"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD18(TEXT("/Game/FX/Frozen/Meshes/head_19.head_19"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD19(TEXT("/Game/FX/Frozen/Meshes/head_20.head_20"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD20(TEXT("/Game/FX/Frozen/Meshes/head_21.head_21"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD21(TEXT("/Game/FX/Frozen/Meshes/head_22.head_22"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD22(TEXT("/Game/FX/Frozen/Meshes/head_23.head_23"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_HEAD23(TEXT("/Game/FX/Frozen/Meshes/head_24.head_24"));
	SM_HEADS.Add(&SM_HEAD1);
	SM_HEADS.Add(&SM_HEAD2);
	SM_HEADS.Add(&SM_HEAD3);
	SM_HEADS.Add(&SM_HEAD4);
	SM_HEADS.Add(&SM_HEAD5);
	SM_HEADS.Add(&SM_HEAD6);
	SM_HEADS.Add(&SM_HEAD7);
	SM_HEADS.Add(&SM_HEAD8);
	SM_HEADS.Add(&SM_HEAD9);
	SM_HEADS.Add(&SM_HEAD10);
	SM_HEADS.Add(&SM_HEAD11);
	SM_HEADS.Add(&SM_HEAD12);
	SM_HEADS.Add(&SM_HEAD13);
	SM_HEADS.Add(&SM_HEAD14);
	SM_HEADS.Add(&SM_HEAD15);
	SM_HEADS.Add(&SM_HEAD16);
	SM_HEADS.Add(&SM_HEAD17);
	SM_HEADS.Add(&SM_HEAD18);
	SM_HEADS.Add(&SM_HEAD19);
	SM_HEADS.Add(&SM_HEAD20);
	SM_HEADS.Add(&SM_HEAD21);
	SM_HEADS.Add(&SM_HEAD22);
	SM_HEADS.Add(&SM_HEAD23);

	for (int i = 0; i < 23; ++i)
	{
		if (!heads[i])
		{
			heads[i] = CreateDefaultSubobject<UStaticMeshComponent>(names[i]);
			if (SM_HEADS[i]->Succeeded())
			{
				heads[i]->SetStaticMesh(SM_HEADS[i]->Object);
				heads[i]->BodyInstance.SetCollisionProfileName(TEXT("NoCollision"));
				heads[i]->SetupAttachment(GetMesh(), TEXT("HeadSocket"));
				heads[i]->SetVisibility(false);
			}
		}
	}
}

void AMyCharacter::FreezeHead()
{
	float WaitTime = 0.1f;
	GetWorld()->GetTimerManager().SetTimer(HeadHandle, FTimerDelegate::CreateLambda([&]()
		{
			MYLOG(Warning, TEXT("%d"), iHeadFrame);

			for (int i = 0; i < 23; ++i)
			{
				if (iHeadFrame == i) {
					FreezeAnimation(heads, iHeadFrame, bHeadAnimEnd);
					break;
				}
			}
		}), WaitTime, true);
}

void AMyCharacter::FreezeAnimation(TArray<UStaticMeshComponent*> bones, int& frame, bool& end)
{
	//투명모드 조정
	if (iHeadFrame != 0)
		bones[frame - 1]->SetVisibility(false);
	bones[frame]->SetVisibility(true);

	//머리만 머티리얼로 점점 얼리기
	if (bones == heads)
		bones[frame]->CreateDynamicMaterialInstance(1)->SetScalarParameterValue(TEXT("Emissive"), frame * 0.125);

	//배열 끝 판정
	if (bones.Num() - 1 == frame)
		end = true;
		//GetWorld()->GetTimerManager().ClearTimer(FreezeHandle);
	else
		frame += 1;
}

float AMyCharacter::Getfspeed()
{
	float speed = fSnowballInitialSpeed + fAimingElapsedTime * fThrowPower;
	return speed;
};