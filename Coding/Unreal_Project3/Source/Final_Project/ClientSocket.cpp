
#include "ClientSocket.h"

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

	// Connect
	//while (true)
	//{
	//	if (::connect(_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	//	{
	//		// 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
	//		if (::WSAGetLastError() == WSAEWOULDBLOCK)
	//			continue;
	//		// 이미 연결된 상태라면 break
	//		if (::WSAGetLastError() == WSAEISCONN)
	//			break;
	//		// Error
	//		break;
	//	}
	//}

	MYLOG(Warning, TEXT("Connected to Server!"));
	return true;
}

void ClientSocket:: ProcessPacket(unsigned char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_PACKET_LOGIN_OK:
	{
	

	}
	break;
	case SC_PACKET_LOGIN_FAIL:
	{

	}
	break;

	case SC_PACKET_PUT_OBJECT:
	{

		break;
	}
	case SC_PACKET_MOVE:
	{
		break;
	}

	case SC_PACKET_REMOVE_OBJECT:
	{

		break;
	}

	case SC_PACKET_CHAT:
	{

		break;
	}
	case SC_PACKET_STATUS_CHANGE:
	{
		ReadyToSend_StatusPacket();

	}
	}
}
//void process_data(char* net_buf, size_t io_byte)
//{
//	char* ptr = net_buf;
//	static size_t in_packet_size = 0;
//	static size_t saved_packet_size = 0;
//	static char packet_buffer[BUF_SIZE];
//
//	while (0 != io_byte) {
//		if (0 == in_packet_size) in_packet_size = ptr[0];
//		if (io_byte + saved_packet_size >= in_packet_size) {
//			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
//			//ProcessPacket(packet_buffer);
//			ptr += in_packet_size - saved_packet_size;
//			io_byte -= in_packet_size - saved_packet_size;
//			in_packet_size = 0;
//			saved_packet_size = 0;
//		}
//		else {
//			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
//			saved_packet_size += io_byte;
//			io_byte = 0;
//		}
//	}
//}

void ClientSocket::ReadyToSend_LoginPacket()
{
	MYLOG(Warning, TEXT("Connected to Server!"));
	cs_packet_login packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_LOGIN;
	strcpy_s(packet.id, _id);
	strcpy_s(packet.pw, _pw);
	size_t sent = 0;
	SendPacket(&packet);
};

void ClientSocket::ReadyToSend_StatusPacket() {
	sc_packet_status_change packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_STATUS_CHANGE;
	SendPacket(&packet);
};

void ClientSocket::SetPlayerController(AMyPlayerController* pPlayerController)
{
	// 플레이어 컨트롤러 세팅
	if (pPlayerController)
	{
		PlayerController = pPlayerController;
	}
}

void ClientSocket::ReadyToSend_MovePacket(char dr)
{
	cs_packet_move packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_MOVE;
	packet.direction = dr;
	size_t sent = 0;
	SendPacket(&packet);
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

uint32 ClientSocket::Run()
{
	// 초기 init 과정을 기다림
	FPlatformProcess::Sleep(0.03);
	// recv while loop 시작
	// StopTaskCounter 클래스 변수를 사용해 Thread Safety하게 해줌
	ReadyToSend_LoginPacket();
	RecvPacket();
	while (StopTaskCounter.GetValue() == 0 && PlayerController != nullptr)
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
