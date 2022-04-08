// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "GameFramework/Actor.h"
#include "MyItemBox.generated.h"

UCLASS()
class FINAL_PROJECT_API AMyItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyItemBox();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

private:
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	

	UPROPERTY(VisibleAnywhere, Category = Box)
	UBoxComponent* trigger;

	UPROPERTY(VisibleAnywhere, Category = Box)
	UStaticMeshComponent* box;

	UPROPERTY(VisibleAnywhere, Category = effect)
	UParticleSystemComponent* effect;

	UPROPERTY(EditInstanceOnly, Category = Box)
	TSubclassOf<class AMyItem> itemClass;

	/*UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* PSystem);*/
};
