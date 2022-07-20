// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "I_Throwable.h"
#include "Iceball.generated.h"

UCLASS()
class FINAL_PROJECT_API AIceball : public AActor, public II_Throwable
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AIceball();
	
	virtual void Tick(float DeltaTime) override;

	// Throwable 인터페이스에서 Throw 이벤트 발생 시 호출
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Itf")
	void Throw(FVector Direction, float Speed);
	virtual void Throw_Implementation(FVector Direction, float Speed) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Itf")
	void IceballThrow(FRotator Rotation, float Speed);
	virtual void IceballThrow_Implementation(FRotator Rotation, float Speed) override;

	// Function that is called when the projectile hits something.
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SpawnIceWall();

public:
	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	USphereComponent* collisionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UStaticMeshComponent* meshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Component)
	UProjectileMovementComponent* projectileMovementComponent;
	
	UPROPERTY(VisibleAnywhere, Category = Class)
	TSubclassOf<class AActor> icewallClass;

private:
	FRotator rotation;
};
