// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "ExtraProjectGameModeBase.h"
#include "ClientSocket.h"
#include "UObject/ConstructorHelpers.h"


#pragma comment(lib, "ws2_32.lib")
ClientSocket* g_socket = nullptr;
void CALLBACK recv_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
	//MYLOG(Warning, TEXT("recv_callback"));
	if (num_byte == 0) {
		recv_flag = 0;
		/*ZeroMemory(&mySocket->_recv_over._wsa_over, sizeof(mySocket->_recv_over._wsa_over));
		mySocket->_recv_over._wsa_buf.buf = reinterpret_cast<char*>(mySocket->_recv_over._net_buf + mySocket->_prev_size);
		mySocket->_recv_over._wsa_buf.len = sizeof(mySocket->_recv_over._net_buf) - mySocket->_prev_size;
		WSARecv(mySocket->_socket, &mySocket->_recv_over._wsa_buf, 1, 0, &recv_flag, &mySocket->_recv_over._wsa_over, recv_callback);*/
		UE_LOG(LogTemp, Warning, TEXT("recv_error "));

		return;
	}
	unsigned char* packet_start = g_socket->_recv_over._net_buf;
	int packet_size = packet_start[0];
	int remain_data = num_byte + g_socket->_prev_size;
	g_socket->process_data(g_socket->_recv_over._net_buf, remain_data);
	recv_flag = 0;
	ZeroMemory(&g_socket->_recv_over._wsa_over, sizeof(g_socket->_recv_over._wsa_over));
	g_socket->_recv_over._wsa_buf.buf = reinterpret_cast<char*>(g_socket->_recv_over._net_buf + g_socket->_prev_size);
	g_socket->_recv_over._wsa_buf.len = sizeof(g_socket->_recv_over._net_buf) - g_socket->_prev_size;
	WSARecv(g_socket->_socket, &g_socket->_recv_over._wsa_buf, 1, 0, &recv_flag, &g_socket->_recv_over._wsa_over, recv_callback);
	SleepEx(0, true);
}
AMyPlayerController::AMyPlayerController()
{	
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FClassFinder<ACharacter> TORNADO_AI(TEXT("/Game/Blueprints/TornadoAI_BP.TornadoAI_BP_C"));
	if (TORNADO_AI.Succeeded() && (TORNADO_AI.Class != nullptr))
	{
		Tornado_AIClass = TORNADO_AI.Class;
	}

}
void AMyPlayerController::OnPossess(APawn* pawn_)
{
	Super::OnPossess(pawn_);
	FPlatformProcess::Sleep(0);
}

void AMyPlayerController::BeginPlay()
{
	g_socket = ClientSocket::GetSingleton();
	g_socket->SetPlayerController(this);
	mySocket = g_socket;
	g_socket->Connect();
	DWORD recv_flag = 0;
	ZeroMemory(&g_socket->_recv_over._wsa_over, sizeof(g_socket->_recv_over._wsa_over));
	g_socket->_recv_over._wsa_buf.buf = reinterpret_cast<char*>(g_socket->_recv_over._net_buf + g_socket->_prev_size);
	g_socket->_recv_over._wsa_buf.len = sizeof(g_socket->_recv_over._net_buf) - g_socket->_prev_size;
	WSARecv(g_socket->_socket, &g_socket->_recv_over._wsa_buf, 1, 0, &recv_flag, &g_socket->_recv_over._wsa_over, recv_callback);
	g_socket->Send_LoginPacket();

	UWorld* World = GetWorld();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), Tornado_AIClass, tonado_ai);

	SleepEx(0, true);
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	mySocket->CloseSocket();
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	int Tid = 0;
	for (auto sd : tonado_ai)
	{

		Tornado_AI = Cast<ACharacter>(sd);
		auto loc = Tornado_AI->GetActorLocation();
		float fNewYaw = Tornado_AI->GetActorRotation().Yaw;		//yaw 값만 필요함
		auto vel = Tornado_AI->GetVelocity();
		mySocket->Send_MovePacket(Tid++, loc, vel);
	}

	SleepEx(0, true);

	
}


