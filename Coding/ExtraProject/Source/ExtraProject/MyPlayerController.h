// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <thread>
#include <atomic>
#include "ExtraProject.h"
#include "GameFramework/PlayerController.h"
#include "ClientSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class EXTRAPROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
		TSubclassOf<class ACharacter> Tornado_AIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class ACharacter* Tornado_AI;	// 토네이도AI

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* pawn_) override;


	void SendPlayerInfo(int input);
	

	// 이동 좌표 전송 속도 조절
	FTimerHandle TornadoMoveHandle;

	ClientSocket* mySocket;


};
