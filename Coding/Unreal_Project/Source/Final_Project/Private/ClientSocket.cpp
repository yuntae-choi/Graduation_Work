
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
		_login_ok = true;

		// 캐릭터 정보
		cCharacter info;
		info.SessionId = packet->s_id;
		info.X = packet->x;
		info.Y = packet->y;
		info.Z = packet->z;
		info.Yaw = packet->yaw;

		CharactersInfo.players[info.SessionId] = info;
		MyPlayerController->SetSessionId(info.SessionId);
		MyPlayerController->SetCharactersInfo(&CharactersInfo);
		MyPlayerController->SetInitInfo(info);

		MYLOG(Warning, TEXT("[Recv login ok] id : %d, location : (%f,%f,%f), yaw : %f"), info.SessionId, info.X, info.Y, info.Z, info.Yaw);

		break;
	}

	case SC_PACKET_LOGIN_FAIL:
		break;

	case SC_PACKET_READY:
	{
		sc_packet_ready* packet = reinterpret_cast<sc_packet_ready*>(ptr);
		int _s_id = packet->s_id;
		MYLOG(Warning, TEXT("[player %d ] : READY"), packet->s_id);
		break;
	}

	case SC_PACKET_START:
	{
		// 게임시작
		
		break;
	}


	case SC_PACKET_PUT_OBJECT:
	{
		sc_packet_put_object* packet = reinterpret_cast<sc_packet_put_object*>(ptr);

		auto info = make_shared<cCharacter>();
		info->SessionId = packet->s_id;
		info->X = packet->x;
		info->Y = packet->y;
		info->Z = packet->z;
		info->Yaw = packet->yaw;

		MyPlayerController->SetNewCharacterInfo(info);

		MYLOG(Warning, TEXT("[Recv put object] id : %d, location : (%f,%f,%f), yaw : %f"), info->SessionId, info->X, info->Y, info->Z, info->Yaw);

		break;
	}

	case SC_PACKET_MOVE:
	{
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(ptr);

		CharactersInfo.players[packet->sessionID].X = packet->x;		// 캐릭터 정보
		CharactersInfo.players[packet->sessionID].Y = packet->y;		// 캐릭터 정보
		CharactersInfo.players[packet->sessionID].Z = packet->z;		// 캐릭터 정보
		CharactersInfo.players[packet->sessionID].Yaw = packet->yaw;		// 캐릭터 정보
		CharactersInfo.players[packet->sessionID].VX = packet->vx;		// 캐릭터 정보
		CharactersInfo.players[packet->sessionID].VY = packet->vy;		// 캐릭터 정보
		CharactersInfo.players[packet->sessionID].VZ = packet->vz;		// 캐릭터 정보
		CharactersInfo.players[packet->sessionID].direction = packet->direction;		// 캐릭터 정보

		//MYLOG(Warning, TEXT("[Recv move] id: %d, location: (%f,%f,%f), yaw: %f, velocity: (%f,%f,%f), dir: %f"), packet->sessionID, packet->x, packet->y, packet->z, packet->yaw, packet->vx, packet->vy, packet->vz, packet->direction);
		break;
	}

	case SC_PACKET_THROW_SNOW:
	{
		cs_packet_throw_snow* packet = reinterpret_cast<cs_packet_throw_snow*>(ptr);
		CharactersInfo.players[packet->s_id].fCx = packet->x;		// 카메라 위치
		CharactersInfo.players[packet->s_id].fCy = packet->y;		// 카메라 위치
		CharactersInfo.players[packet->s_id].fCz = packet->z;		// 카메라 위치
		CharactersInfo.players[packet->s_id].fCDx = packet->dx;		// 카메라 방향
		CharactersInfo.players[packet->s_id].fCDy = packet->dy;		// 카메라 방향
		CharactersInfo.players[packet->s_id].fCDz = packet->dz;		// 카메라 방향

		//MYLOG(Warning, TEXT("[Recv throw snow] id : %d, cam_loc : (%f, %f, %f), cam_dir : (%f, %f, %f)"), packet->s_id, packet->x, packet->y, packet->z, packet->dx, packet->dy, packet->dz);
		MyPlayerController->SetNewBall(packet->s_id);

		break;
	}

	case SC_PACKET_HP:
	{
		sc_packet_hp_change* packet = reinterpret_cast<sc_packet_hp_change*>(ptr);

		CharactersInfo.players[packet->s_id].HealthValue = packet->hp;

		//MYLOG(Warning, TEXT("[Recv hp change] id : %d, hp : %d"), packet->s_id, packet->hp);

		break;

	}

	case SC_PACKET_REMOVE_OBJECT:
	{
		cs_packet_throw_snow* packet = reinterpret_cast<cs_packet_throw_snow*>(ptr);

		MYLOG(Warning, TEXT("[Recv remove object] id : %d"), packet->s_id);

		break;
	}

	case SC_PACKET_STATUS_CHANGE:
	{
		sc_packet_status_change* packet = reinterpret_cast<sc_packet_status_change*>(ptr);
		MYLOG(Warning, TEXT("[Recv status change] id : %d, state : %d"), packet->s_id, packet->state);
		if (ST_SNOWMAN == packet->state) {
			CharactersInfo.players[packet->s_id].My_State = ST_SNOWMAN;
			//MYLOG(Warning, TEXT("snowMAN !!! [ %d ] "), CharactersInfo.players[packet->s_id].HealthValue);
		}
		else if (ST_ANIMAL == packet->state) {
			CharactersInfo.players[packet->s_id].My_State = ST_ANIMAL;
		}
		break;
	}

	//case SC_PACKET_CHAT:
	//{

	//	break;
	//}
	//case SC_PACKET_ATTACK:
	//{

	//	cs_packet_attack* packet = reinterpret_cast<cs_packet_attack*>(ptr);
	//	MYLOG(Warning, TEXT("player%d attack "), packet->s_id);
	//	break;
	//}
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

	auto player_ = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(MyPlayerController, 0));
	if (!player_) return;
	auto location_ = player_->GetActorLocation();

	packet.z = location_.Z;
	size_t sent = 0;


	MYLOG(Warning, TEXT("[Send login] z : %f"), packet.z);
	SendPacket(&packet);
	
};

void ClientSocket::Send_ReadyPacket()
{
	cs_packet_ready packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_READY;
	SendPacket(&packet);

};


void ClientSocket::Send_StatusPacket(STATE_Type _state) {
	//CharactersInfo.players[iMy_s_id].My_State = _state;
	sc_packet_status_change packet;
	packet.size = sizeof(packet);
	packet.s_id = MyPlayerController->iSessionId;
	packet.type = CS_PACKET_STATUS_CHANGE;
	packet.state = _state;

	MYLOG(Warning, TEXT("[Send status] status : %d"), _state);
	SendPacket(&packet);
};

void ClientSocket::Send_DamagePacket() {
	cs_packet_damage packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_DAMAGE;

	MYLOG(Warning, TEXT("[Send damage]"));
	SendPacket(&packet);
};

void ClientSocket::Send_MovePacket(int s_id, FVector MyLocation, FRotator MyRotation, FVector MyVelocity, float dir)
{
	if (_login_ok) {
		cs_packet_move packet;
		packet.size = sizeof(packet);
		packet.type = CS_PACKET_MOVE;
		packet.sessionID = s_id;
		packet.x = MyLocation.X;
		packet.y = MyLocation.Y;
		packet.z = MyLocation.Z;
		packet.yaw = MyRotation.Yaw;
		packet.vx = MyVelocity.X;
		packet.vy = MyVelocity.Y;
		packet.vz = MyVelocity.Z;
		packet.direction = dir;

		//MYLOG(Warning, TEXT("[Send move] id: %d, location: (%f,%f,%f), yaw: %f, velocity: (%f,%f,%f), dir: %f"), s_id, packet.x, packet.y, packet.z, packet.yaw, packet.vx, packet.vy, packet.vz, dir);
		SendPacket(&packet);
	}
};


void ClientSocket::Send_Throw_Packet(int s_id, FVector MyLocation, FVector MyDirection)
{

	cs_packet_throw_snow packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_THROW_SNOW;
	packet.s_id = s_id;
	packet.x = MyLocation.X;
	packet.y = MyLocation.Y;
	packet.z = MyLocation.Z;
	packet.dx = MyDirection.X;
	packet.dy = MyDirection.Y;
	packet.dz = MyDirection.Z;
	size_t sent = 0;

	//MYLOG(Warning, TEXT("[Send throw snow] id: %d, loc: (%f, %f, %f), dir: (%f, %f, %f)"), s_id, packet.x, packet.y, packet.z, packet.dx, packet.dy, packet.dz);
	SendPacket(&packet);
};

//void ClientSocket::ReadyToSend_AttackPacket()
//{
//
//	cs_packet_attack packet;
//	packet.size = sizeof(packet);
//	packet.type = CS_PACKET_ATTACK;
//	//packet.s_id = iMy_s_id;
//	size_t sent = 0;
//	SendPacket(&packet);
//};

void ClientSocket::Send_ItemPacket(int item_no)
{

	cs_packet_get_item packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_GET_ITEM;
	//packet.s_id = iMy_s_id;
	packet.item_no = item_no;
	size_t sent = 0;
	SendPacket(&packet);
};

void ClientSocket::Send_ChatPacket(int sessionID, float x, float y, float z)
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

void ClientSocket::Send_LogoutPacket(const int& s_id)
{
	cs_packet_logout packet;
	packet.size = sizeof(packet);
	packet.type = CS_PACKET_LOGOUT;
	packet.s_id = MyPlayerController->iSessionId;
	MYLOG(Warning, TEXT("[Send Logout] id : %d"), packet.s_id);
	SendPacket(&packet);
}


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

	
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_socket), h_iocp, 0, 0);
	
	 RecvPacket();

	Send_LoginPacket();

	FPlatformProcess::Sleep(0.03);

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
