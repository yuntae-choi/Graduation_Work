// Fill out your copyright notice in the Description page of Project Settings.


#include "SnowMan.h"

// Sets default values
ASnowMan::ASnowMan()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 1000.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SNOWMAN(TEXT("/Game/Characters/Snowman/snowman.snowman"));
	if (SK_SNOWMAN.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_SNOWMAN.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> SNOWMAN_ANIM(TEXT("/Game/Animations/Snowman/SnowMan_AnimBP.SnowMan_AnimBP_C"));
	if (SNOWMAN_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(SNOWMAN_ANIM.Class);
	}

}

// Called when the game starts or when spawned
void ASnowMan::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASnowMan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASnowMan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

