// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Bonfire.generated.h"

UCLASS()
class FINAL_PROJECT_API ABonfire : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABonfire();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateOverlapCharacters(); // overlap 중인 캐릭터를 찾아서 체온 증감 상태를 변경

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// 내부 - 회복량, 외부 - 피해량
	static const int iHealAmount;
	static const int iDamageAmount;
private:
	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	USphereComponent* sphereComponent;
};
