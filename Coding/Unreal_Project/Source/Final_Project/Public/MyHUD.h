// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class FINAL_PROJECT_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	// Primary draw call for the HUD.
	virtual void DrawHUD() override;

protected:
	// This will be drawn at the center of the screen.
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* crosshairTexture;
	
};
