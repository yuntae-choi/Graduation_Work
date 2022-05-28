// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
//#include "NiagaraComponent.h"
#include "Tornado.generated.h"

UCLASS()
class FINAL_PROJECT_API ATornado : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATornado();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UCapsuleComponent* collisionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	class UNiagaraComponent* tornadoNiagara;
};
