// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SupplyBox.generated.h"

UCLASS()
class FINAL_PROJECT_API ASupplyBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASupplyBox();
	int32 GetId() const { return iSpBoxId; }
	void SetId(int32 id) { iSpBoxId = id; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	static int iIdCountHelper;
	int32 iSpBoxId;

};
