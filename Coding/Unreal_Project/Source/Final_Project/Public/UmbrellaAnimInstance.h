// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UmbrellaAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FINAL_PROJECT_API UUmbrellaAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UUmbrellaAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	bool GetOpen() { return bOpen; };
	void SetOpen(bool value) { bOpen = value; };
	bool GetClose() { return bClose; };
	void SetClose(bool value) { bClose = value; };

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bOpen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bClose;
};
