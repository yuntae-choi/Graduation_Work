// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Final_Project.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "EditorManager.generated.h"

UCLASS()
class FINAL_PROJECT_API AEditorManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEditorManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component)
	UNiagaraSystem* crumbleNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component)
	UParticleSystem* snowSplash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component)
	UMaterial* snowPaint;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PlaySnowballCrumbleEffect(FVector pos);

};
