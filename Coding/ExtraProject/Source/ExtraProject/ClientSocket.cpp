// Fill out your copyright notice in the Description page of Project Settings.



#include "ClientSocket.h"
#include "MyPlayerController.h"

ClientSocket::~ClientSocket()
{
	closesocket(_socket);
	WSACleanup();
}

bool ClientSocket::Connect()
{
	
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(SERVER_PORT);

	int ret = connect(_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
		return false;
	
	MYLOG(Warning, TEXT("Connected to Server!"));
	return true;
}

void ClientSocket::ProcessPacket(unsigned char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_PACKET_LOGIN_OK:
	{
		sc_packet_login_ok* packet = reinterpret_cast<sc_packet_login_ok*>(ptr);

		my_s_id = packet->s_id;
		_login_ok = true;
		break;
	}

	case SC_PACKET_LOGIN_FAIL:
		break;

	case SC_PACKET_MOVE:
	{
		break;
	}
	case SC_PACKET_START:
	{
		// 게임시작
		_start_ok = true;
		break;
	}
	
	}
}

void ClientSocket::SetPlayerController(AMyPlayerController* pPlayerController)
{
	// 플레이어 컨트롤러 세팅
	if (pPlayerController)
	{
		MyPlayerController = pPlayerController;
	}
}

void ClientSocket::Send_LoginPacket()
{
	cs_packet_login packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_LOGIN;
	strcpy_s(packet.id, _id);
	strcpy_s(packet.pw, _pw);
	size_t sent = 0;


	//MYLOG(Warning, TEXT("[Send login] z : %f"), packet.z);
	SendPacket(&packet);

};






void ClientSocket::Send_MovePacket(int id,  FVector MyLocation, FVector MyVelocity)
{
	if (_login_ok) {
		cs_packet_move packet;
		packet.size = sizeof(packet);
		packet.type = CS_PACKET_NPC_MOVE;
		packet.sessionID = id + MAX_USER;
		packet.x = MyLocation.X;
		packet.y = MyLocation.Y;
		packet.z = MyLocation.Z;
		packet.yaw = 0;
		packet.vx = MyVelocity.X;
		packet.vy = MyVelocity.Y;
		packet.vz = MyVelocity.Z;
		packet.direction = 0;

		//MYLOG(Warning, TEXT("[Send move]  location: (%f,%f,%f)"),  packet.x, packet.y, packet.z);
		SendPacket(&packet);
	}
};


void ClientSocket::CloseSocket()
{
	closesocket(_socket);
	WSACleanup();
}

bool ClientSocket::Init()
{
	return true;
}

void ClientSocket::process_data(unsigned char* net_buf, size_t io_byte)
{
	//MYLOG(Warning, TEXT("process_data!"));

	unsigned char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static unsigned char packet_buffer[BUF_SIZE];

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
	if (io_byte == 0)
		_prev_size = 0;
}

uint32 ClientSocket::Run()
{
	// 초기 init 과정을 기다림
	FPlatformProcess::Sleep(0.03);

	//Connect();
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_socket), h_iocp, 0, 0);

	RecvPacket();

	Send_LoginPacket();

	SleepEx(0, true);

	// recv while loop 시작
	// StopTaskCounter 클래스 변수를 사용해 Thread Safety하게 해줌
	while (StopTaskCounter.GetValue() == 0 && MyPlayerController != nullptr)
	{
		DWORD num_byte;
		LONG64 iocp_key;
		WSAOVERLAPPED* p_over;

		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_byte, (PULONG_PTR)&iocp_key, &p_over, INFINITE);

		Overlap* exp_over = reinterpret_cast<Overlap*>(p_over);

		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			if (exp_over->_op == OP_SEND)
				delete exp_over;
			continue;
		}

		switch (exp_over->_op) {
		case OP_RECV: {
			if (num_byte == 0) {
				//Disconnect();
				continue;
			}
			int remain_data = num_byte + _prev_size;
			unsigned char* packet_start = exp_over->_net_buf;
			int packet_size = packet_start[0];
			while (packet_size <= remain_data) {
				ProcessPacket(packet_start);
				remain_data -= packet_size;
				packet_start += packet_size;
				if (remain_data > 0) packet_size = packet_start[0];
				else break;
			}

			if (0 < remain_data) {
				_prev_size = remain_data;
				memcpy(&exp_over->_net_buf, packet_start, remain_data);
			}

			RecvPacket();
			SleepEx(0, true);
			break;
		}
		case OP_SEND: {
			if (num_byte != exp_over->_wsa_buf.len) {
				//Disconnect();
			}
			delete exp_over;
			break;
		}

		}

	}
	return 0;
}

void ClientSocket::Stop()
{
	// thread safety 변수를 조작해 while loop 가 돌지 못하게 함
	StopTaskCounter.Increment();
}

void ClientSocket::Exit()
{
}


bool ClientSocket::StartListen()
{
	// 스레드 시작
	if (Thread != nullptr) return false;
	Thread = FRunnableThread::Create(this, TEXT("ClientSocket"), 0, TPri_BelowNormal);
	return (Thread != nullptr);
}

void ClientSocket::StopListen()
{
	// 스레드 종료
	Stop();
	Thread->WaitForCompletion();
	Thread->Kill();
	delete Thread;
	Thread = nullptr;
	StopTaskCounter.Reset();
}
