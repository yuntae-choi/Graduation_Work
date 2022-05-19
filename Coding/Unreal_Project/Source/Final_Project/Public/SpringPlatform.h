// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "SpringPlatform.generated.h"

enum SpringPlatformState {
	Normal, Increasing, Decreasing, Maximum
};

UCLASS()
class FINAL_PROJECT_API ASpringPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpringPlatform();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	int GetSpringPlatformState() { return iSpringPlatformState; };
	void SetSpringPlatformState(int springPlatformState) { iSpringPlatformState = springPlatformState; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void UpdateSpring();

public:

private:
	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	USceneComponent* sceneComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* springMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* platformMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UBoxComponent* boxComponent;

	int iSpringPlatformState;
	float fSpringScaleZ;
	float fPlatformLocationZ;
};
