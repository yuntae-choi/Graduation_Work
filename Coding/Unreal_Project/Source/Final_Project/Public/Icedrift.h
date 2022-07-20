// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Icedrift.generated.h"

UCLASS()
class FINAL_PROJECT_API AIcedrift : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIcedrift();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	float GetFarmDuration() { return fFarmDuration; };
	void SetFarmDuration(float duration) { fFarmDuration = duration; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	static const float fFarmDurationMax;	// 얼음 무더기 파밍에 걸리는 시간
	static const int iNumOfIceball;		// 획득할 수 있는 아이스볼 수

	static int iIdCountHelper;
private:
	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UBoxComponent* boxComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* meshComponent;

	UPROPERTY(VisibleAnywhere, Category = Farm)
	float fFarmDuration;	// 파밍에 걸리는 시간 중 현재 남은 시간
};
