// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CameraDirector.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACameraDirector::ACameraDirector()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
    Super::BeginPlay();

    MyPlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
    CameraTwo = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(MyPlayerController, 0));

    SetCamera();
}

// Called every frame
void ACameraDirector::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	//TimeToNextCameraChange -= DeltaTime;
	//if (TimeToNextCameraChange < 0.0f)
	//{
	//	bCameraChanged = true;
	//}

    ChangeCamera();
}

void ACameraDirector::SetCamera()
{
    if (CameraOne)
    {
        MyPlayerController->SetViewTarget(CameraOne);
    }
}

void ACameraDirector::ChangeCamera()
{
    const float SmoothBlendTime = 0.75f;

	if (MyPlayerController && MyPlayerController->bInGame)
	{
		if (CameraTwo && (MyPlayerController->GetViewTarget() == CameraOne))
		{
			MyPlayerController->SetViewTargetWithBlend(CameraTwo, SmoothBlendTime);
		}
	}

	//bCameraChanged = false;
}