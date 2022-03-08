//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//
//#include "ClientSocket.h"
//#include <sstream>
//#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
//#include "Runtime/Core/Public/HAL/RunnableThread.h"
//#include <algorithm>
//#include <string>
//
//
//ClientSocket::~ClientSocket()
//{
//
//	closesocket(_socket);
//	WSACleanup();
//}
//
//bool ClientSocket::InitSocket()
//{
//	WSADATA wsaData;
//	// 윈속 버전을 2.2로 초기화
//	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (nRet != 0) {
//		return false;
//	}
//
//	// TCP 소켓 생성	
//	_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
//	if (_socket == INVALID_SOCKET) {
//		return false;
//	}
//
//	return true;
//}
//
//bool ClientSocket::Connect(const char* pszIP, int nPort)
//{
//	// 접속할 서버 정보를 저장할 구조체
//	SOCKADDR_IN stServerAddr;
//
//	stServerAddr.sin_family = AF_INET;
//	// 접속할 서버 포트 및 IP
//	stServerAddr.sin_port = htons(nPort);
//	stServerAddr.sin_addr.s_addr = inet_addr(pszIP);
//
//	int nRet = connect(_socket, (sockaddr*)&stServerAddr, sizeof(sockaddr));
//	if (nRet == SOCKET_ERROR) {
//		return false;
//	}
//
//	return true;
//}
//
////bool ClientSocket::Connect()
////{
////	SOCKADDR_IN server_addr;
////	ZeroMemory(&server_addr, sizeof(server_addr));
////	server_addr.sin_family = AF_INET;
////	server_addr.sin_port = htons(SERVER_PORT);
////	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
////
////	int nRet = connect(_socket, (sockaddr*)&server_addr, sizeof(sockaddr));
////	if (nRet == SOCKET_ERROR) {
////		return false;
////	}
////
////	return true;
////}
//
//void ClientSocket::SendPacket(void* packet) {
//	int psize = reinterpret_cast<unsigned char*>(packet)[0];
//	int ptype = reinterpret_cast<unsigned char*>(packet)[1];
//	OverlappedEx* over = new OverlappedEx;
//	over->event_type = OP_SEND;
//	memcpy(over->IOCP_buf, packet, psize);
//	ZeroMemory(&over->over, sizeof(over->over));
//	over->wsabuf.buf = reinterpret_cast<CHAR*>(over->IOCP_buf);
//	over->wsabuf.len = psize;
//	int ret = WSASend(_socket, &over->wsabuf, 1, NULL, 0,
//		&over->over, NULL);
//};
//
//void ClientSocket::send_login_packet()
//{
//	cs_packet_login packet;
//	packet.size = sizeof(packet);
//	packet.type = CS_PACKET_LOGIN;
//	strcpy_s(packet.name, _name.c_str());
//	size_t sent = 0;
//	SendPacket(&packet);
//};
//
//void ClientSocket::send_move_packet(char dr)
//{
//	cs_packet_move packet;
//	packet.size = sizeof(packet);
//	packet.type = CS_PACKET_MOVE;
//	packet.direction = dr;
//	size_t sent = 0;
//	SendPacket(&packet);
//}
//
//void ClientSocket::process_data(char* net_buf, size_t io_byte)
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
//			ProcessPacket(packet_buffer);
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
//
//void ClientSocket::ProcessPacket(char* ptr)
//{
//	static bool first_time = true;
//	switch (ptr[1])
//	{
//	case SC_PACKET_LOGIN_OK:
//	{
//		//printf("완료");
//		_login_ok = true;
//		sc_packet_login_ok* packet = reinterpret_cast<sc_packet_login_ok*>(ptr);
//		_myid = packet->id;
//		m_x = packet->x;
//		m_y = packet->y;
//		//_x_origin = packet->x - SCREEN_WIDTH / 2;
//		//_y_origin = packet->y - SCREEN_WIDTH / 2;
//		//_max_hp = packet->maxhp;
//		//_m_hp = &avatar._max_hp;
//		//_hp = packet->hp;
//		//_hp = &_hp;
//		//move(packet->x, packet->y);
//		//show();
//	}
//	break;
//	case SC_PACKET_LOGIN_FAIL:
//	{
//
//		//printf("아이디 다시 입력");
//		send_login_packet();
//	}
//	break;
//
//	case SC_PACKET_PUT_OBJECT:
//	{
//
//		break;
//	}
//	case SC_PACKET_MOVE:
//	{
//		sc_packet_move* my_packet = reinterpret_cast<sc_packet_move*>(ptr);
//
//		break;
//	}
//
//	case SC_PACKET_REMOVE_OBJECT:
//	{
//		sc_packet_remove_object* my_packet = reinterpret_cast<sc_packet_remove_object*>(ptr);
//
//		break;
//	}
//
//	case SC_PACKET_CHAT:
//	{
//		sc_packet_chat* my_packet = reinterpret_cast<sc_packet_chat*>(ptr);
//
//		break;
//	}
//	case SC_PACKET_STATUS_CHANGE:
//	{
//		sc_packet_status_change* packet = reinterpret_cast<sc_packet_status_change*>(ptr);
//		break;
//	}
//
//	default:
//		//printf("Unknown PACKET type [%d]\n", ptr[1]);
//	}
//}
//
////uint32 ClientSocket::Run()
////{
////	// 초기 init 과정을 기다림
////	FPlatformProcess::Sleep(0.03);
////	// recv while loop 시작
////	// StopTaskCounter 클래스 변수를 사용해 Thread Safety하게 해줌
////	//while (StopTaskCounter.GetValue() == 0 && PlayerController != nullptr)
////	//{
////	//	stringstream RecvStream;
////	//	int PacketType;
////	//	int nRecvLen = recv(
////	//		ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0
////	//	);
////	//	if (nRecvLen > 0)
////	//	{
////	//		// 패킷 처리
////	//		RecvStream << recvBuffer;
////	//		RecvStream >> PacketType;
////
////	//		switch (PacketType)
////	//		{
////	//		case EPacketType::RECV_PLAYER:
////	//		{
////	//			PlayerController->RecvWorldInfo(RecvCharacterInfo(RecvStream));
////	//		}
////	//		break;
////	//		case EPacketType::CHAT:
////	//		{
////	//			PlayerController->RecvChat(RecvChat(RecvStream));
////	//		}
////	//		break;
////	//		case EPacketType::ENTER_NEW_PLAYER:
////	//		{
////	//			PlayerController->RecvNewPlayer(RecvNewPlayer(RecvStream));
////	//		}
////	//		break;
////	//		case EPacketType::SYNC_MONSTER:
////	//		{
////	//			PlayerController->RecvMonsterSet(RecvMonsterSet(RecvStream));
////	//		}
////	//		break;
////	//		case EPacketType::SPAWN_MONSTER:
////	//		{
////	//			PlayerController->RecvSpawnMonster(RecvMonster(RecvStream));
////	//		}
////	//		break;
////	//		case EPacketType::DESTROY_MONSTER:
////	//		{
////	//			PlayerController->RecvDestroyMonster(RecvMonster(RecvStream));
////	//		}
////	//		break;
////	//		default:
////	//			break;
////	//		}
////	//	}
////	//}
////	return 0;
////}