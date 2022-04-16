
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

void ClientSocket::ProcessPacket(unsigned char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_PACKET_LOGIN_OK:
	{

		sc_packet_login_ok* packet = reinterpret_cast<sc_packet_login_ok*>(ptr);
		int id = packet->s_id;
		PlayerController->UpdatePlayerS_id(id);
		_login_ok = true;
		ReadyToSend_StatusPacket();
		//ReadyToSend_MovePacket(packet->s_id, fMy_x, fMy_y, fMy_z);

				// 캐릭터 정보
		cCharacter p;
		p.SessionId = packet->s_id;
		p.X = packet->x;;
		p.Y = packet->y;
		p.Z = packet->z;
		CharactersInfo.players[packet->s_id] = p;		// 캐릭터 정보
		PlayerController->iMySessionId = packet->s_id;
		PlayerController->bSetPlayer = true;
		PlayerController->StartPlayerInfo(CharactersInfo.players[packet->s_id]);
		PlayerController->RecvWorldInfo(&CharactersInfo);

		//MYLOG(Warning, TEXT("i'm player%d init spawn : (%f, %f, %f)"), packet->s_id, x, y, z);

	}
	break;
	case SC_PACKET_LOGIN_FAIL:
	{

	}
	break;

	case SC_PACKET_PUT_OBJECT:
	{
		
		sc_packet_put_object* packet = reinterpret_cast<sc_packet_put_object*>(ptr);
		int id = packet->s_id;
		float x = packet->x;
		float y = packet->y;
		float z = packet->z;
		ReadyToSend_ChatPacket(packet->s_id, x, y, z);

		cCharacter p;
		p.SessionId = packet->s_id;
		p.X = packet->x;;
		p.Y = packet->y;
		p.Z = packet->z;
		CharactersInfo.players[packet->s_id] = p;		// 캐릭터 정보
		
		PlayerController->RecvNewPlayer(p);
	   // PlayerController->UpdateNewPlayer(packet->s_id, x, y, z);

		break;
	}
	case SC_PACKET_MOVE:
	{
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(ptr);

		CharactersInfo.players[packet->sessionID].X = packet->x;;		// 캐릭터 정보
		CharactersInfo.players[packet->sessionID].Y = packet->y;;		// 캐릭터 정보
		CharactersInfo.players[packet->sessionID].Z = packet->z;;		// 캐릭터 정보

		break;
	}

	case SC_PACKET_HP:
	{
		sc_packet_hp_change* packet = reinterpret_cast<sc_packet_hp_change*>(ptr);
		int target = packet->target;
		
		//이런식으로 클라이언트info 관리하는 벡터 만들면 인덱스 접근 해서 바꿔줘
		CharactersInfo.players[target].HealthValue = packet->hp;


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


void ClientSocket::ReadyToSend_LoginPacket()
{
	MYLOG(Warning, TEXT("Connected to Server!"));
	cs_packet_login packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_LOGIN;
	strcpy_s(packet.id, _id);
	strcpy_s(packet.pw, _pw);
	packet.x = fMy_x;
	packet.y = fMy_y;
	packet.z = fMy_z;
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

void ClientSocket::ReadyToSend_MovePacket(int sessionID, float x, float y, float z)
{
	if (_login_ok) {
		cs_packet_move packet;
		packet.size = sizeof(packet);
		packet.type = CS_PACKET_MOVE;
		//packet.direction = dr;
		packet.sessionID = sessionID;
		packet.x = x;
		packet.y = y;
		packet.z = z;

		size_t sent = 0;
		auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		packet.move_time = millisec_since_epoch;
		SendPacket(&packet);
	}
};

void ClientSocket::ReadyToSend_AttackPacket()
{

	cs_packet_attack packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_ATTACK;
	//packet.direction = dr;
	size_t sent = 0;
	SendPacket(&packet);
};

void ClientSocket::ReadyToSend_ChatPacket(int sessionID, float x, float y, float z)
{

	cs_packet_chat packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_CHAT;
	packet.s_id = sessionID;
	packet.x = x;
	packet.y = y;
	packet.z = z;

	//packet.direction = dr;
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
	Connect();
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_socket), h_iocp, 0, 0);
	
	RecvPacket();
	_login_ok = false;
	ReadyToSend_LoginPacket();
	FPlatformProcess::Sleep(0.1);
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
