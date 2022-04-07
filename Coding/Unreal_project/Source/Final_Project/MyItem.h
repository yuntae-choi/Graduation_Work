// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "GameFramework/Actor.h"
#include "MyItem.generated.h"

UCLASS()
class FINAL_PROJECT_API AMyItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyItem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = Umbrella)
	UStaticMeshComponent* umbrella;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
