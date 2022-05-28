// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Final_Project.h"
#include "MyPlayerController.h"
#include "MyCharacter.h"
#include "GameFramework/Actor.h"
#include "CameraDirector.generated.h"

UCLASS()
class FINAL_PROJECT_API ACameraDirector : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ACameraDirector();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere)
        AActor* CameraOne;


    AMyCharacter* CameraTwo;

    AMyPlayerController* MyPlayerController;	// 플레이어 컨트롤러 정보	

	float TimeToNextCameraChange = 2.0f;
    bool bCameraChanged = false;

    void SetCamera();
    void ChangeCamera();
};