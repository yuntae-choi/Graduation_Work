// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Snowdrift.generated.h"

UCLASS()
class FINAL_PROJECT_API ASnowdrift : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASnowdrift();

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
	static const float fFarmDurationMax;	// 눈 무더기 파밍에 걸리는 시간
	static const int iNumOfSnowball;		// 획득할 수 있는 눈덩이 수

	static int iIdCountHelper;
	int iId;
private:	
	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UBoxComponent* boxComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* meshComponent;

	UPROPERTY(VisibleAnywhere, Category = Farm)
	float fFarmDuration;	// 파밍에 걸리는 시간 중 현재 남은 시간
};
