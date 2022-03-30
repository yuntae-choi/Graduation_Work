// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FMyCharacterData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	FMyCharacterData() : sessionID(1), init_x(100), MaxHP(100.0f), Attack(10.0f),
						 maxSnowBalls(3), nHasSnowBalls(0), hasUmbrella(false), hasBag(false),
						 maxMatches(3), nHasMatches(0), maxSnowBalls_byABag(5){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 sessionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float init_x;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float maxSnowBalls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int nHasSnowBalls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool hasUmbrella;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool hasBag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int maxMatches;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int nHasMatches;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int maxSnowBalls_byABag;
};


/**
 * 
 */
UCLASS()
class FINAL_PROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMyGameInstance();

	virtual void Init() override;
	FMyCharacterData* GetMyCharacterData(int32 sessionID);

private:
	UPROPERTY()
	class UDataTable* MyCharacterTable;
};
