// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorManager.h"

// Sets default values
AEditorManager::AEditorManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEditorManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEditorManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEditorManager::PlaySnowballCrumbleEffect(FVector pos)
{
	if (crumbleNiagara) {
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), crumbleNiagara, pos, FRotator(1), FVector(1), true, true, ENCPoolMethod::AutoRelease, true);
		//NiagaraComp->SetNiagaraVariableFloat(FString("StrengthCoef"), CoefStrength);
	}
}

