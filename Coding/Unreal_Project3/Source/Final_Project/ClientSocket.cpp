
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

void ClientSocket::SendPacket(void* packet)
{
	int psize = reinterpret_cast<unsigned char*>(packet)[0];
	int ptype = reinterpret_cast<unsigned char*>(packet)[1];
	OverlappedEx* over = new OverlappedEx;
	over->event_type = OP_SEND;
	memcpy(over->IOCP_buf, packet, psize);
	ZeroMemory(&over->over, sizeof(over->over));
	over->wsabuf.buf = reinterpret_cast<CHAR*>(over->IOCP_buf);
	over->wsabuf.len = psize;
	int ret = WSASend(_socket, &over->wsabuf, 1, NULL, 0,
		&over->over, NULL);
}

void ClientSocket::ReadyToSend_LoginPacket()
{
	cs_packet_login packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_LOGIN;
	strcpy_s(packet.name, _name.c_str());
	size_t sent = 0;
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