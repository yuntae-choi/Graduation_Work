// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"
#include "MyHUD.h"

AMyGameModeBase::AMyGameModeBase()
{
	static ConstructorHelpers::FClassFinder<AHUD> CROSSHAIR_HUD(TEXT("/Game/Blueprints/BP_FPSHUD.BP_FPSHUD_C"));


	DefaultPawnClass = AMyCharacter::StaticClass();
	//DefaultPawnClass = ASnowMan::StaticClass();
	PlayerControllerClass = AMyPlayerController::StaticClass();
	HUDClass = CROSSHAIR_HUD.Class;
}