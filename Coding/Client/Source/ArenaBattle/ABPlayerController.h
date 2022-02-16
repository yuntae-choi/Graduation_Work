// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerController.h"
#include "ClientSocket.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AABPlayerController();
	virtual ~AABPlayerController();

	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;
	
protected:
	virtual void BeginPlay() override;

	ClientSocket*		Socket;			// 서버와 접속할 소켓
	bool				bIsConnected;	// 서버와 접속 유무
	int					SessionId;		// 캐릭터의 세션 고유 아이디 (랜덤값)
	cCharactersInfo*	CharactersInfo;	// 다른 캐릭터의 정보
};
