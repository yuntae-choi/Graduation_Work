// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ClientSocket.h"

#pragma comment(lib, "ws2_32.lib")


AMyPlayerController::AMyPlayerController()
{

	_cs = ClientSocket::GetSingleton();
	/*_cs->h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_cs->_socket), _cs->h_iocp, 0, 0);
	*///int ret = _cs->Connect();
	//if (ret)
	//{
		//UE_LOG(LogClass, Log, TEXT("IOCP Server connect success!"));
		_cs->SetPlayerController(this);
		
	//}


	PrimaryActorTick.bCanEverTick = true;

	bNewPlayerEntered = false;
}


void AMyPlayerController::BeginPlay()
{
	//Super::BeginPlay(); //게임 종료가 안됨

	//auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	//if (!m_Player)
	//	return;
	//auto MyLocation = m_Player->GetActorLocation();
	//auto MyRotation = m_Player->GetActorRotation();
	_cs->_my_x = MyLocation.X;
	_cs->_my_y = MyLocation.Y; 
	_cs->_my_z = MyLocation.Z;
	_cs->StartListen();
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	_cs->CloseSocket();
	_cs->StopListen();
	//Super::EndPlay(EndPlayReason);
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bNewPlayerEntered)
		UpdateNewPlayer();

	//UpdateRotation();
}


void AMyPlayerController::UpdatePlayerInfo(int input)
{
	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	_my_session_id = m_Player->iSessionID;
	auto MyLocation = m_Player->GetActorLocation();
	auto MyRotation = m_Player->GetActorRotation();
	if (input == COMMAND_MOVE)
		_cs->ReadyToSend_MovePacket(_my_session_id, MyLocation.X, MyLocation.Y, MyLocation.Z);
	else if (input == COMMAND_ATTACK)
		_cs->ReadyToSend_AttackPacket();

}

void AMyPlayerController::UpdatePlayerS_id(int id)
{
	_my_session_id = id;
	auto m_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!m_Player)
		return;
	m_Player->iSessionID = id;
	m_Player->SetActorLocationAndRotation(FVector(id * 100.0f, id * 100.0f, m_Player->GetActorLocation().Z), FRotator(0.0f, -90.0f, 0.0f));
}

void AMyPlayerController::RecvNewPlayer(int sessionID, float x, float y, float z)
{
	MYLOG(Warning, TEXT("recv ok player%d : %f, %f, %f"), sessionID, x, y, z);

	UWorld* World = GetWorld();

	bNewPlayerEntered = true;
	_other_session_id = sessionID;
	_other_x = x;
	_other_y = y;
	_other_z = z;
}

void AMyPlayerController::UpdateNewPlayer()
{
	UWorld* const World = GetWorld();

	// 새로운 플레이어가 자기 자신이면 무시
	if (_other_session_id == _my_session_id)
	{
		bNewPlayerEntered = false;
		return;
	}

	bNewPlayerEntered = true;

	// 새로운 플레이어를 필드에 스폰
	FVector SpawnLocation_;
	SpawnLocation_.X = _other_x;
	SpawnLocation_.Y = _other_y;
	SpawnLocation_.Z = _other_z;

	FRotator SpawnRotation;
	SpawnRotation.Yaw = 0.0f;
	SpawnRotation.Pitch = 0.0f;
	SpawnRotation.Roll = 0.0f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	//SpawnParams.Name = FName(*FString(to_string(_other_session_id).c_str()));

	TSubclassOf<class AMyCharacter> WhoToSpawn;
	WhoToSpawn = AMyCharacter::StaticClass();
	AMyCharacter* SpawnCharacter = World->SpawnActor<AMyCharacter>(WhoToSpawn, SpawnLocation_, SpawnRotation, SpawnParams);

	if (nullptr == SpawnCharacter)
	{
		MYLOG(Warning, TEXT("spawn fail"));
		return;
	}
	MYLOG(Warning, TEXT("spawn ok player%d : %f, %f, %f"), _other_session_id, _other_x, _other_y, _other_z);

	SpawnCharacter->SpawnDefaultController();
	SpawnCharacter->iSessionID = _other_session_id;

	bNewPlayerEntered = false;
}

//void AMyPlayerController::UpdateRotation()
//{
//	float pitch, yaw, roll;
//	UKismetMathLibrary::BreakRotator(GetControlRotation(), roll, pitch, yaw);
//	pitch = UKismetMathLibrary::ClampAngle(pitch, -15.0f, 30.0f);
//	FRotator newRotator = UKismetMathLibrary::MakeRotator(roll, pitch, yaw);
//	SetControlRotation(newRotator);
//}
