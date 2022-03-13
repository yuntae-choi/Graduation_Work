
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

	// Connect
	while (true)
	{
		if (::connect(_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			// 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			// 이미 연결된 상태라면 break
			if (::WSAGetLastError() == WSAEISCONN)
				break;
			// Error
			break;
		}
	}

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
		ReadyToSend_MovePacket(1);

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


void ClientSocket::ReadyToSend_MovePacket(char dr)
{
	cs_packet_move packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_MOVE;
	packet.direction = dr;
	size_t sent = 0;
	SendPacket(&packet);
};


