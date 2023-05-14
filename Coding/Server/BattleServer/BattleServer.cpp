#include "pch.h"
#include "BattleServer.h"
#include "P_Manager.h"


using namespace std;
default_random_engine Dre;
uniform_int_distribution<> Uid;// 범위 지정
uniform_int_distribution<> Sprang;// 보급 박스 범위 지정

BattleServer::BattleServer() 
{
};


bool BattleServer::Connect()
{
	cout << "Connected begin!" << endl;
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		cout << "Failed to start wsa" << endl;

	Lobby_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(SERVER_PORT);

	int ret = connect(Lobby_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
		return false;
	state = SERVER_FREE;
	cout << "Connected to Server!" << endl;
	_recv_over._op = OP_SERVER_RECV;
	_recv_over._wsa_buf.buf = reinterpret_cast<char*>(_recv_over._net_buf);
	_recv_over._wsa_buf.len = sizeof(_recv_over._net_buf);
	ZeroMemory(&_recv_over._wsa_over, sizeof(_recv_over._wsa_over));
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(Lobby_socket), g_h_iocp, 0, 0);
	Lobby_do_recv();
	return true;
}

void BattleServer::Run()
{
	PM = std::make_unique<PManager>();
	PM->g_timer = CreateEvent(NULL, FALSE, FALSE, NULL);
	Start();
	timer_thread = thread([this]() {ev_timer(); });
};

void BattleServer::End()
{
	JoinThread();
	timer_thread.join();
	CloseHandle(g_h_iocp);
	CloseHandle(PM->g_timer);
	closesocket(sever_socket);
};

//void LobbyServer::init_server()
//{
//	for (int i = 0; i < MAX_B_SERVER; i++)
//	{
//		BattleServers[i] = new BattleServer();
//	}
//};

bool BattleServer::OnRecv(int s_id, Overlap* exp_over, DWORD num_byte)
{
	if (num_byte == 0) {
		cout << "연결종료" << endl;
		Disconnect(s_id);
		return false;
	}
	CLIENT& cl = clients[s_id];
	int remain_data = num_byte + cl._prev_size;
	unsigned char* packet_start = exp_over->_net_buf;
	int packet_size = packet_start[0];

	while (packet_size <= remain_data) {
		PM->ProcessPacket(s_id, packet_start);
		remain_data -= packet_size;
		packet_start += packet_size;
		if (remain_data > 0) packet_size = packet_start[0];
		else break;
	}

	if (0 < remain_data) {
		cl._prev_size = remain_data;
		memcpy(&exp_over->_net_buf, packet_start, remain_data);
	}
	if (remain_data == 0)
		cl._prev_size = 0;
	cl.do_recv();
	return true;
};
bool BattleServer::OnAccept(Overlap* exp_over)
{
	DWORD dwBytes;
	cout << "Accept Completed.\n";
	SOCKET c_socket = *(reinterpret_cast<SOCKET*>(exp_over->_net_buf));


	int n__s_id = PM->get_id();
	if (-1 == n__s_id) {
		cout << "user over.\n";
	}
	else {
		CLIENT& cl = clients[n__s_id];
		cl.state_lock.lock();
		cl._s_id = n__s_id;
		cl.cl_state = ST_ACCEPT;
		cl.state_lock.unlock();
		cl._prev_size = 0;
		cl._recv_over._op = OP_RECV;
		cl._recv_over._wsa_buf.buf = reinterpret_cast<char*>(cl._recv_over._net_buf);
		cl._recv_over._wsa_buf.len = sizeof(cl._recv_over._net_buf);
		ZeroMemory(&cl._recv_over._wsa_over, sizeof(cl._recv_over._wsa_over));
		cl._socket = c_socket;
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_h_iocp, n__s_id, 0);
		cl.do_recv();
	}

	ZeroMemory(&exp_over->_wsa_over, sizeof(exp_over->_wsa_over));
	c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	*(reinterpret_cast<SOCKET*>(exp_over->_net_buf)) = c_socket;
	AcceptEx(sever_socket, c_socket, exp_over->_net_buf + 8, 0, sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16, &dwBytes, &exp_over->_wsa_over);
	return true;

};

bool BattleServer::OnEvent(int s_id, Overlap* exp_over)
{
	switch (exp_over->_op) {

	case OP_PLAYER_HEAL: {
		if (clients[s_id]._hp + 10 <= clients[s_id]._max_hp) {
			clients[s_id]._hp += 10;
			PM->Heal(s_id, BONFIRE);
		}
		else
			clients[s_id]._hp = clients[s_id]._max_hp;
		send_hp_packet(s_id);
		break;

	}
	case OP_PLAYER_DAMAGE: {
		if (clients[s_id].bIsSnowman) break;
		if (clients[s_id].is_bone == false) {
			if (clients[s_id]._hp - 1 > clients[s_id]._min_hp) {
				clients[s_id]._hp -= 1;
				PM->ColdDamage(s_id);
				send_hp_packet(s_id);
			}
			else if (clients[s_id]._hp - 1 == clients[s_id]._min_hp)
			{
				//cout << "모닥불 데미지 눈사람" << endl;
				clients[s_id].iCurrentSnowballCount = 0;
				clients[s_id].iCurrentIceballCount = 0;
				clients[s_id].iCurrentMatchCount = 0;
				clients[s_id].iMaxSnowballCount = clients[s_id].iOriginMaxSnowballCount;
				clients[s_id].iMaxIceballCount = clients[s_id].iOriginMaxIceballCount;
				clients[s_id].iMaxMatchCount = clients[s_id].iOriginMaxMatchCount;
				clients[s_id].bHasBag = false;
				clients[s_id].bHasShotGun = false;
				clients[s_id].bHasUmbrella = false;
				clients[s_id].bIsSnowman = true;
				clients[s_id]._hp -= 1;
				send_hp_packet(s_id);
				for (auto& other : clients) {
					if (ST_INGAME != other.cl_state) continue;
					send_state_change(s_id, other._s_id, ST_SNOWMAN);
					send_kill_log(other._s_id, -2, s_id, DeathByCold);
				}

				cout << "플레이어" << s_id << "가 모닥불에 의해 눈사람으로 변함" << endl;

				int cnt = 0;
				int target_s_id;
				for (auto& other : clients) {
					if (s_id == other._s_id) continue;
					if (ST_INGAME != other.cl_state) continue;
					if (false == other.bIsSnowman)
					{
						cnt++;
						target_s_id = other._s_id;
					}
				}
				if (cnt == 1) {
					for (auto& other : clients) {
						if (ST_INGAME != other.cl_state)
							continue;
						send_game_end(target_s_id, other._s_id);
					}
					send_server_game_end(target_s_id);

					cout << "게임종료" << endl;
				}
				else {
					int bear_cnt = 0;
					int snowman_cnt = 0;;
					for (auto& other : clients) {
						if (ST_INGAME != other.cl_state) continue;
						if (false == other.bIsSnowman)
						{
							bear_cnt++;
						}
						else if (true == other.bIsSnowman)
						{
							snowman_cnt++;
						}
					}
					for (auto& other : clients) {
						if (ST_INGAME != other.cl_state)
							continue;
						send_player_count(other._s_id, bear_cnt, snowman_cnt);
					}
				}
			}
		}
		break;
	}
	case OP_OBJ_SPAWN: {
		cout << "OP_OBJ_SPAWN " << endl;

		float f_x = Sprang(Dre) - 10000.0f;
		float f_y = Sprang(Dre) - 10000.0f;

		//printf("SupplyBOX %f, %f\n" , f_x, f_y);
		sc_packet_put_object packet;
		for (auto& other : clients) {
			if (ST_INGAME != other.cl_state) continue;
			if (ST_TORNADO == other.pl_state) continue;
			packet.size = sizeof(packet);
			packet.type = SC_PACKET_PUT_OBJECT;
			packet.object_type = SUPPLYBOX;
			packet.x = f_x;
			packet.y = f_y;
			packet.z = 4500.0f;
			size_t sent = 0;
			other.do_send(sizeof(packet), &packet);
		}
		PM->PutSupplyBox();
		break;
	}
	case OP_SERVER_RECYCLE: {
		send_server_restart(s_id);
		ReStart();
		cout << "OP_SERVER_RECYCLE " << endl;
		break;
	}

	}

	return true;
};

void BattleServer::ReStart()
{
	PM->g_timer = CreateEvent(NULL, FALSE, FALSE, NULL);
	reset_server();
	HANDLE _timer;
	PM->timer_reset = true;
	for (int i = 0; i < MAX_SNOWDRIFT; ++i) GA.g_snow_drift[i] = true;
	for (int i = 0; i < MAX_SNOWDRIFT; ++i) GA.g_ice_drift[i] = true;
	for (int i = 0; i < MAX_ITEM; ++i) GA.g_item[i] = true;
	for (int i = 0; i < MAX_ITEM; ++i) GA.g_spitem[i] = true;
	for (int i = 0; i < MAX_USER; ++i) clients[i]._s_id = i;
}

//플레이어 이벤트 등록
void BattleServer::PutEvent(int target, int player_id, COMMAND type)
{
	if (type == OP_OBJ_SPAWN)
		cout << "Player_Event " << type << endl;
	Overlap* exp_over = new Overlap;
	exp_over->_op = type;
	exp_over->_target = player_id;
	PostQueuedCompletionStatus(g_h_iocp, 1, target, &exp_over->_wsa_over);
}



//타이머
void BattleServer::ev_timer()
{
	while (true) {
		WaitForSingleObject(PM->g_timer, INFINITE);
		{
			PM->timer_q.clear();
		}
		PM->timer_reset = false;
		PM->PutSupplyBox();
		while (!PM->timer_reset) {
			timer_ev order;
			PM->timer_q.try_pop(order);
			//auto t = order.start_t - chrono::system_clock::now();
			int s_id = order.this_id;
			int target_id = order.target_id;
			if (s_id == Gm_id)
			{
				if (order.start_t <= chrono::system_clock::now()) {
					if (order.order == SP_DROP) {
						cout << "ev_timer " << endl;
						PutEvent(0, 0, OP_OBJ_SPAWN);
						this_thread::sleep_for(50ms);
					}
					else if (order.order == GAME_OVER) {
						cout << "ev_timer " << endl;
						PutEvent(0, 0, OP_SERVER_RECYCLE);
						this_thread::sleep_for(50ms);
					}
				}
				else {
					PM->timer_q.push(order);
					this_thread::sleep_for(10ms);
				}
			}
			else {
				if (false == is_player(s_id)) continue;
				if (clients[s_id].cl_state != ST_INGAME) continue;
				if (clients[s_id]._is_active == false) continue;
				if (order.start_t <= chrono::system_clock::now()) {
					if (order.order == CL_BONEFIRE) {
						if (clients[s_id].is_bone == false) continue;
						PutEvent(s_id, order.target_id, OP_PLAYER_HEAL);
						this_thread::sleep_for(50ms);
					}
					else if (order.order == CL_BONEOUT) {
						if (clients[s_id].is_bone == true) continue;
						PutEvent(s_id, order.target_id, OP_PLAYER_DAMAGE);
						this_thread::sleep_for(50ms);
					}
					else if (order.order == CL_MATCH) {
						PutEvent(s_id, order.target_id, OP_PLAYER_HEAL);
						this_thread::sleep_for(50ms);
					}
					else if (order.order == CL_END_MATCH) {
						send_is_bone_packet(s_id);
						this_thread::sleep_for(50ms);
					}
				}
				else {
					PM->timer_q.push(order);
					this_thread::sleep_for(10ms);
				}
			}
		}
	}
}


bool BattleServer::Lobby_recv(Overlap* exp_over, DWORD num_byte)
{
	if (num_byte == 0) {
		cout << "연결종료" << endl;
		return false;
	}
	int remain_data = num_byte + _prev_size;
	unsigned char* packet_start = exp_over->_net_buf;
	int packet_size = packet_start[0];

	while (packet_size <= remain_data) {
		PM->ProcessServerPacket(packet_start);
		remain_data -= packet_size;
		packet_start += packet_size;
		if (remain_data > 0) packet_size = packet_start[0];
		else break;
	}

	if (0 < remain_data) {
		_prev_size = remain_data;
		memcpy(&exp_over->_net_buf, packet_start, remain_data);
	}
	if (remain_data == 0)
		_prev_size = 0;
	Lobby_do_recv();
	return true;
}

void BattleServer::Lobby_do_recv()
{
	DWORD recv_flag = 0;
	ZeroMemory(&_recv_over._wsa_over, sizeof(_recv_over._wsa_over));
	_recv_over._op = OP_SERVER_RECV;
	_recv_over._wsa_buf.buf = reinterpret_cast<char*>(_recv_over._net_buf + _prev_size);
	_recv_over._wsa_buf.len = sizeof(_recv_over._net_buf) - _prev_size;
	int ret = WSARecv(Lobby_socket, &_recv_over._wsa_buf, 1, 0, &recv_flag, &_recv_over._wsa_over, NULL);
	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
		if (ERROR_IO_PENDING != error_num)
			error_display(error_num);
	}
}

void BattleServer::Lobby_send(int num_bytes, void* mess)
{
	
}

void BattleServer::Lobby_do_send(int num_bytes, void* mess)
{
	Overlap* ex_over = new Overlap(COMMAND::OP_SERVER_SEND, num_bytes, mess);
	int ret = WSASend(Lobby_socket, &ex_over->_wsa_buf, 1, 0, 0, &ex_over->_wsa_over, NULL);
	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
		if (ERROR_IO_PENDING != error_num)
			error_display(error_num);
	}
}

void BattleServer::Lobby_Login_SEND()
{
	ss_packet_server_login packet;
	packet.size = sizeof(packet);
	packet.type = SS_PACKET_SERVER_LOGIN;
	Lobby_do_send(sizeof(packet), &packet);
}