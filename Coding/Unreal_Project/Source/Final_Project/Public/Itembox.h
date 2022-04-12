// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "MyCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Itembox.generated.h"

enum ItemboxState {
	Closed, Opening, Opened
};

enum ItemTypeList {
	Match, Umbrella, Bag
};

UCLASS()
class FINAL_PROJECT_API AItembox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItembox();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	int GetItemboxState() { return iItemboxState; };
	void SetItemboxState(int itemboxState) { iItemboxState = itemboxState; };
	void UpdateRotation(float DeltaTime);
	int GetItemType() { return iItemType; };
	void DeleteItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

private:
	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UBoxComponent* boxComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* meshComponentDown;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* meshComponentUp;

	UPROPERTY(VisibleAnyWhere, Category = Component)
	UStaticMeshComponent* itemMeshComponent;

	int iItemboxState;
	float fSumRotation;
	int iItemType;
};
