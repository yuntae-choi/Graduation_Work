// Copyright Epic Games, Inc. All Rights Reserved.


#include "ExtraProjectGameModeBase.h"
#include "MyPlayerController.h"

AExtraProjectGameModeBase::AExtraProjectGameModeBase()
{
	//static ConstructorHelpers::FClassFinder<AHUD> CROSSHAIR_HUD(TEXT("/Game/Blueprints/BP_FPSHUD.BP_FPSHUD_C"));


	//DefaultPawnClass = AMyCharacter::StaticClass();
	//DefaultPawnClass = ASnowMan::StaticClass();
	PlayerControllerClass = AMyPlayerController::StaticClass();
	
}