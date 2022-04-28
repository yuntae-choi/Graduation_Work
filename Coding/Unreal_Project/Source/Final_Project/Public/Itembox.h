// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Itembox.generated.h"

enum ItemboxState {
	Closed, Opening, Opened, Empty
};

enum ItemTypeList {
	Match, Umbrella, Bag, Random
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
	void SetItem(int itemType = 3);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	int RandomItemType();	// 각 아이템별 확률에 따라 랜덤한 아이템 타입 반환

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> itemMeshArray;		// 아이템별 스테틱메시를 담는 배열

	UPROPERTY(VisibleAnyWhere)
	int iItemType;	// 아이템박스에 들어있는 아이템 타입
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
};
