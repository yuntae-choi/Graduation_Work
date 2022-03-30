// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

UMyGameInstance::UMyGameInstance()
{
	FString CharacterDataPath = TEXT("/Game/GameData/MyCharacterData.MyCharacterData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_MYCHARACTER(*CharacterDataPath);
	MYCHECK(DT_MYCHARACTER.Succeeded());
	MyCharacterTable = DT_MYCHARACTER.Object;
}

void UMyGameInstance::Init()
{
	Super::Init();
}

FMyCharacterData* UMyGameInstance::GetMyCharacterData(int32 sessionID)
{
	return MyCharacterTable->FindRow<FMyCharacterData>(*FString::FromInt(sessionID), TEXT(""));
}