//----------------------------------------------------------------------------------------------------------------------------------------------
// GameServer.cpp 파일
//----------------------------------------------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "CorePch.h"
#include "CLIENT.h"
#include "Overlap.h"
#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"
#include "GameDataBase.h"
#include <string>


HANDLE g_h_iocp;
HANDLE g_timer;
SOCKET sever_socket;

condition_variable cv;
default_random_engine dre;
uniform_int_distribution<> uid{ 0,7 };// 범위 지정

LockQueue<timer_ev> timer_q;

using namespace std;


void Wsa_err_display(int err_no);
void Init_Pos(int s_id, char* _id, char* _pw, float _z);
void Init_Arr();
void rand_arr(int* r_arr);
void Player_Event(int target, int player_id, COMMAND type);
void player_heal(int s_id);
void player_damage(int s_id);
int get_id();
void Accept_Player(int _s_id);
void Disconnect(int _s_id);
void Timer_Event(int np_s_id, int user_id, int ev, std::chrono::milliseconds ms);
void process_packet(int _s_id, unsigned char* p);
void worker_thread();
void ev_timer();

int main()
{
	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	int ret = WSAStartup(MAKEWORD(2, 2), &WSAData);
	Wsa_err_display(ret);
	sever_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sever_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(sever_socket, SOMAXCONN);

	g_h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(sever_socket), g_h_iocp, 0, 0);

	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	char   accept_buf[sizeof(SOCKADDR_IN) * 2 + 32 + 100];
	Overlap   accept_ex;
	*(reinterpret_cast<SOCKET*>(&accept_ex._net_buf)) = c_socket;
	ZeroMemory(&accept_ex._wsa_over, sizeof(accept_ex._wsa_over));
	accept_ex._op = OP_ACCEPT;
	AcceptEx(sever_socket, c_socket, accept_buf, 0, sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16, NULL, &accept_ex._wsa_over);

	g_timer = CreateEvent(NULL, FALSE, FALSE, NULL);
	Init_Arr();
	vector <thread> worker_threads;
	thread timer_thread{ ev_timer };
	// 시스템 정보 가져옴
	//SYSTEM_INFO sysInfo;
	//GetSystemInfo(&sysInfo);
	//printf_s("[INFO] CPU 쓰레드 갯수 : %d\n", sysInfo.dwNumberOfProcessors);
	// 적절한 작업 스레드의 갯수는 (CPU * 2) + 1
	//int nThreadCnt = sysInfo.dwNumberOfProcessors;
	int nThreadCnt = 5;
	for (int i = 0; i < 10; ++i)
		worker_threads.emplace_back(worker_thread);
	for (auto& th : worker_threads)
		th.join();
	timer_thread.join();
	for (auto& cl : clients) {
		if (ST_INGAME == cl._state)
			Disconnect(cl._s_id);
	}
	closesocket(sever_socket);
	WSACleanup();
}

void Wsa_err_display(int err_no)
{
	if (err_no != 0)
	{
		switch (err_no)

		{

		case WSASYSNOTREADY:

			printf("네트워크 접속을 준비 못함");

			break;

		case WSAVERNOTSUPPORTED:

			printf("요구한 윈속 버전이 서포트 안됨");

			break;

		case WSAEINPROGRESS:

			printf("블로킹 조작이 실행중");

			break;

		case WSAEPROCLIM:

			printf("최대 윈속 프로세스 처리수 초과");

			break;

		case WSAEFAULT:

			printf("두번째 인수의 포인터가 무효");

			break;

		}

	}
}

void Init_Arr()
{
	for (int i = 0; i < MAX_SNOWDRIFT; ++i) GA.g_snow_drift[i] = true;
	for (int i = 0; i < MAX_SNOWDRIFT; ++i) GA.g_ice_drift[i] = true;
	for (int i = 0; i < MAX_ITEM; ++i) GA.g_item[i] = true;
	for (int i = 0; i < MAX_USER; ++i) clients[i]._s_id = i;
}

void Init_Pos(int s_id, char* _id, char* _pw, float _z)
{
	CLIENT& cl = clients[s_id];
	strcpy_s(cl._id, _id);
	strcpy_s(cl._pw, _pw);
	cl._state = ST_INGAME;
	cl.x = 600.0f * cos(s_id + 45.0f);
	cl.y = 600.0f * sin(s_id + 45.0f);
	cl.z = _z;
	cl.Yaw = s_id * 55.0f - 115.0f;
	if (cl.Yaw > 180) cl.Yaw -= 360;
	cl._hp = cl._max_hp;
}

void rand_arr(int* r_arr) {
	int cnt = 0;
	int test[MAX_BULLET_RANG];
	memset(test, 0, MAX_BULLET_RANG * 4);
	while (cnt < MAX_BULLET_RANG) {
		int32 num = uid(dre);
		if (test[num] == 0) {
			test[num] = 1;
			r_arr[cnt] = num;
			cnt++;
		}
	}
}

void player_heal(int s_id)
{
	if (false == clients[s_id].bIsSnowman) {
		if (clients[s_id]._hp < clients[s_id]._max_hp) {
			Timer_Event(s_id, s_id, CL_BONEFIRE, 1000ms);
		}
	}
}

void player_damage(int s_id)
{
	//if (!clients[s_id].dot_dam) {
	if (false == clients[s_id].bIsSnowman) {
		if (clients[s_id]._hp > clients[s_id]._min_hp) {
			Timer_Event(s_id, s_id, CL_BONEOUT, 1000ms);;
		}
	}
	//clients[s_id].dot_dam = true;
//}
}


//새로운 id(인덱스) 할당
int get_id()
{
	static int g_id = 0;

	for (int i = 0; i < MAX_USER; ++i) {
		clients[i].state_lock.lock();
		if (ST_FREE == clients[i]._state) {
			clients[i]._state = ST_ACCEPT;
			clients[i].state_lock.unlock();
			return i;
		}
		else clients[i].state_lock.unlock();
	}
	cout << "Maximum Number of Clients Overflow!!\n";
	return -1;
}

//해제
void Disconnect(int _s_id)
{
	CLIENT& cl = clients[_s_id];
	//cl.vl.lock();
	//unordered_set <int> my_vl = cl.viewlist;
	//cl.vl.unlock();

	strcpy_s(clients[_s_id]._id, " ");

	/*for (auto& other : my_vl) {
		CLIENT& target = clients[other];

		if (ST_INGAME != target._state)
			continue;
		target.vl.lock();
		if (0 != target.viewlist.count(_s_id)) {
			target.viewlist.erase(_s_id);
			target.vl.unlock();
			send_remove_object(other, _s_id);
		}
		else target.vl.unlock();
	}*/
	for (auto& other : clients) {
		if (other._s_id == _s_id) continue;
		other.state_lock.lock();
		if (ST_INGAME != other._state) {
			other.state_lock.unlock();
			continue;
		}
		else other.state_lock.unlock();

		//if (false == is_near(other._id, client_id))
			//continue;

		//other.vl.lock();
		//other.viewlist.insert(s_id);
		//other.vl.unlock();
		cs_packet_logout packet;
		packet.size = sizeof(packet);
		packet.type = SC_PACKET_LOGOUT;
		packet.s_id = cl._s_id;
		other.do_send(sizeof(packet), &packet);
	}
	//clients[_s_id].state_lock.lock();
	clients[_s_id]._state = ST_FREE;

	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER 로 설정
										// 강제 종료 시킨다. 데이터 손실이 있을 수 있음

	stLinger.l_onoff = true;
	// _socket 소켓의 데이터 송수신을 모두 중단
	shutdown(clients[_s_id]._socket, SD_BOTH);
	// 소켓 옵션을 설정
	setsockopt(clients[_s_id]._socket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
	//소켓 연결을 종료
	clients[_s_id].data_init();
	closesocket(clients[_s_id]._socket);
	clients[_s_id]._socket = INVALID_SOCKET;
	//clients[_s_id].state_lock.unlock();

	cout << "------------플레이어 " << _s_id << " 연결 종료------------" << endl;
}

//플레이어 이벤트 등록
void Player_Event(int target, int player_id, COMMAND type)
{
	Overlap* exp_over = new Overlap;
	exp_over->_op = type;
	exp_over->_target = player_id;
	PostQueuedCompletionStatus(g_h_iocp, 1, target, &exp_over->_wsa_over);
}

//타이머 큐 등록
void Timer_Event(int np_s_id, int user_id, int ev, std::chrono::milliseconds ms)
{
	timer_ev order;
	order.this_id = np_s_id;
	order.target_id = user_id;
	order.order = ev;
	order.start_t = chrono::system_clock::now() + ms;
	timer_q.Push(order);
}

//플레이어 접속 정보 전송
void Accept_Player(int _s_id)
{
	CLIENT& cl = clients[_s_id];
	// 새로 접속한 플레이어의 정보를 주위 플레이어에게 보낸다
	for (auto& other : clients) {
		if (other._s_id == _s_id) continue;
		if (other._s_id == GA.g_tonardo_id) continue;
		other.state_lock.lock();
		if (ST_INGAME != other._state) {
			other.state_lock.unlock();
			continue;
		}
		else other.state_lock.unlock();

		sc_packet_put_object packet;
		packet.s_id = cl._s_id;
		strcpy_s(packet.name, cl.name);
		packet.size = sizeof(packet);
		packet.type = SC_PACKET_PUT_OBJECT;
		packet.x = cl.x;
		packet.y = cl.y;
		packet.z = cl.z;
		packet.yaw = cl.Yaw;
		packet.object_type = PLAYER;
		strcpy_s(packet.name, cl._id);
		other.do_send(sizeof(packet), &packet);
	}

	// 새로 접속한 플레이어에게 주위 객체 정보를 보낸다
	for (auto& other : clients) {
		if (other._s_id == _s_id) continue;
		if (ST_INGAME != other._state) {
			if (GA.g_tonardo) {
				if (GA.g_tonardo_id == other._s_id) {
					//토네이도 생성위치 보내기
					sc_packet_put_object packet;
					packet.s_id = other._s_id;
					packet.size = sizeof(packet);
					packet.type = SC_PACKET_PUT_OBJECT;
					packet.x = other.x;
					packet.y = other.y;
					packet.z = other.z;
					packet.yaw = 0.0f;
					packet.object_type = TONARDO;
					printf_s("[토네이도 생성] id : %d, location : (%f,%f,%f), yaw : %f\n", packet.s_id, packet.x, packet.y, packet.z, packet.yaw);
					cl.do_send(sizeof(packet), &packet);
				}
			}
		}
		else {
			sc_packet_put_object packet;
			packet.s_id = other._s_id;
			strcpy_s(packet.name, other.name);
			packet.size = sizeof(packet);
			packet.type = SC_PACKET_PUT_OBJECT;
			packet.x = other.x;
			packet.y = other.y;
			packet.z = other.z;
			packet.yaw = other.Yaw;
			packet.object_type = PLAYER;
			strcpy_s(packet.name, other._id);
			cl.do_send(sizeof(packet), &packet);
		}
	}
}

//패킷 판별
void process_packet(int s_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	CLIENT& cl = clients[s_id];

	switch (packet_type) {
	case CS_PACKET_LOGIN: {
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p);
		bool _OverLap = false;
		if (strcmp("Tornado", packet->id) != 0) {
			if (strcmp("testuser", packet->id) != 0)
			{
				CLIENT& cl = clients[s_id];
				printf_s("[Recv login] ID : %s, PASSWORD : %s, z : %f\n", packet->id, packet->pw, packet->z);

				for (int i = 0; i < MAX_USER; ++i) {
					clients[i].state_lock.lock();
					if (ST_INGAME == clients[i]._state) {
						if (strcmp(packet->id, clients[i]._id) == 0) {
							cout << packet->id << "접속중인 플레이어" << endl;
							send_login_fail_packet(s_id, OVERLAP_AC);
							_OverLap = true;
							clients[i].state_lock.unlock();
							break;
						}
					}
					clients[i].state_lock.unlock();
				}

				if (_OverLap == true) break;
				if (DB_odbc(s_id, packet->id, packet->pw) == true)
				{
					Init_Pos(s_id, packet->id, packet->pw, packet->z);
					printf(" [DB_odbc] 플레이어[id] - ID : %s 로그인 성공 \n", cl._id);
					send_login_ok_packet(s_id);
					Accept_Player(s_id);
				}
				else
				{
					if (DB_id(packet->id) == true) {
						cout << "플레이어[" << s_id << "]" << " 잘못된 비번" << endl;
						send_login_fail_packet(s_id, WORNG_PW);
						break;
					}
					else {
						cout << "플레이어[" << s_id << "]" << " 잘못된 아이디" << endl;
						send_login_fail_packet(s_id, WORNG_ID);
						break;
					}
				}
			}
			else
			{

				string gm_id(packet->id);
				gm_id += to_string(s_id);
				Init_Pos(s_id, (char*)gm_id.c_str(), packet->pw, packet->z);
				printf(" [GM] 플레이어[id] - ID : %s 로그인 성공 \n", cl._id);
				send_login_ok_packet(s_id);
				Accept_Player(s_id);
			}
		}
		else
		{
			GA.g_tonardo_id = true;
			GA.g_tonardo_id = cl._s_id;
			send_login_ok_packet(s_id);
			//cout << "토네이도" << endl;
		}
		break;
	}
	case CS_PACKET_ACCOUNT: {
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p);
		bool _OverLap = false;
		if (strcmp("Tornado", packet->id) != 0) {
			CLIENT& cl = clients[s_id];
			printf_s("[Recv login] ID : %s, PASSWORD : %s\n", packet->id, packet->pw);
			if (_OverLap == true) break;
			if (DB_id(packet->id) == true) {
				cout << "플레이어[" << s_id << "] 계정 생성 실패 - " << "중복된 아이디" << endl;
				send_login_fail_packet(s_id, OVERLAP_ID);
			}
			else
			{
				//DB에 계정등록 
				strcpy_s(cl._id, packet->id);
				strcpy_s(cl._pw, packet->pw);
				DB_save(s_id);
				send_login_fail_packet(s_id, CREATE_AC);
				cout << packet->id << "DB에 계정등록" << endl;
			}
		}
		break;
	}
	case CS_PACKET_MOVE: {

		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(p);
		if (packet->sessionID != GA.g_tonardo_id) {
			CLIENT& cl = clients[packet->sessionID];
			cl.x = packet->x;
			cl.y = packet->y;
			cl.z = packet->z;
			cl.Yaw = packet->yaw;
			cl.VX = packet->vx;
			cl.VY = packet->vy;
			cl.VZ = packet->vz;
			cl.direction = packet->direction;

			unordered_set <int> near_list;
			for (auto& other : clients) {
				if (other._s_id == s_id)
					continue;
				if (ST_INGAME != other._state)
					continue;
				if (false == is_near(s_id, other._s_id))
					continue;

				near_list.insert(other._s_id);
			}

			cl.vl.lock();

			unordered_set <int> my_vl{ cl.viewlist };

			cl.vl.unlock();

			for (auto& other : clients) {
				if (other._s_id == s_id)
					continue;
				if (ST_INGAME != other._state)
					continue;
				send_move_packet(other._s_id, cl._s_id);
			}
		}
		else
		{
			cl.x = packet->x;
			cl.y = packet->y;
			cl.z = packet->z;
			cl.VX = packet->vx;
			cl.VY = packet->vy;
			cl.VZ = packet->vz;
			if (GA.g_start_game) {
				// cout << "토네이도 move" << endl;
				for (auto& other : clients) {
					if (other._s_id == s_id)
						continue;
					if (ST_INGAME != other._state)
						continue;
					cs_packet_move packet;
					packet.sessionID = GA.g_tonardo_id;
					packet.size = sizeof(packet);
					packet.type = SC_PACKET_MOVE;
					packet.x = cl.x;
					packet.y = cl.y;
					packet.z = cl.z;
					packet.vx = cl.VX;
					packet.vy = cl.VY;
					packet.vz = cl.VZ;
					other.do_send(sizeof(packet), &packet);
					//cout << "토네이도 보낼 플레이어" << other._s_id << endl;
				}
			}
		}
		break;
	}
	case CS_PACKET_ATTACK: {
		cs_packet_attack* packet = reinterpret_cast<cs_packet_attack*>(p);
		for (auto& other : clients) {
			if (ST_INGAME != other._state)
				continue;
			packet->type = SC_PACKET_ATTACK;
			//cout << "플레이어[" << packet->s_id << "]가" << "플레이어[" << other._s_id << "]에게 보냄" << endl;
			other.do_send(sizeof(*packet), packet);
			//cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;
		}
		cout << "attack " << packet->bullet << endl;
		break;

	}

	case CS_PACKET_GUNATTACK: {
		cs_packet_attack* packet = reinterpret_cast<cs_packet_attack*>(p);
		for (auto& other : clients) {
			if (ST_INGAME != other._state)
				continue;
			packet->type = SC_PACKET_GUNATTACK;
			//cout << "플레이어[" << packet->s_id << "]가" << "플레이어[" << other._s_id << "]에게 보냄" << endl;
			other.do_send(sizeof(*packet), packet);
			//cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;
		}
		printf("gunattack\n");
		break;

	}
	case CS_PACKET_DAMAGE: {
		if (cl.bIsSnowman) break;
		cout << "플레이어 " << cl._s_id << "데미지 받음 " << endl;
		int current_hp = cl._hp;
		cl._hp -= 30;
		if (cl._hp < 270) cl._hp = 270;
		send_hp_packet(cl._s_id);

		if (current_hp == cl._max_hp && cl.is_bone == true)
		{
			cl._is_active = true;
			player_heal(cl._s_id);
		}

		if (cl._hp <= cl._min_hp)
		{
			cl.iCurrentSnowballCount = 0;
			cl.iCurrentIceballCount = 0;
			cl.iCurrentMatchCount = 0;
			cl.iMaxSnowballCount = cl.iOriginMaxSnowballCount;
			cl.iMaxMatchCount = cl.iOriginMaxMatchCount;
			cl.bHasBag = false;
			cl.bHasUmbrella = false;
			cl.bIsSnowman = true;
			sc_packet_status_change _packet;
			_packet.size = sizeof(_packet);
			_packet.type = SC_PACKET_STATUS_CHANGE;
			_packet.state = ST_SNOWMAN;
			_packet.s_id = s_id;
			for (auto& other : clients) {
				if (ST_INGAME != other._state)
					continue;
				other.do_send(sizeof(_packet), &_packet);
				//cout << "눈사람" << endl;
				//	cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;
			}
			int cnt = 0;
			int target_s_id;
			for (auto& other : clients) {
				if (cl._s_id == other._s_id) continue;
				if (ST_INGAME != other._state) continue;
				if (false == other.bIsSnowman)
				{
					cnt++;
					//cout << "눈덩이 cnt" << endl;

					target_s_id = other._s_id;
				}
			}
			if (cnt == 1) {
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					send_game_end(target_s_id, other._s_id);
				}
				cout << "게임종료" << endl;
			}
		}

		break;

	}
	case CS_PACKET_MATCH: {
		cout << "플레이어 " << cl._s_id << ": 성냥 사용 " << endl;
		if (cl.bIsSnowman) break;
		if (cl.iCurrentMatchCount > 0)
		{
			cl.iCurrentMatchCount--;
			if (cl._hp + 30 > cl._max_hp)
				cl._hp = cl._max_hp;
			else
				cl._hp += 30;
			send_hp_packet(cl._s_id);
			//cl.is_bone = true;
			//Timer_Event(s_id, s_id, CL_MATCH, 1000ms);
			//Timer_Event(s_id, s_id, CL_MATCH, 2000ms);
			//Timer_Event(s_id, s_id, CL_MATCH, 3000ms);
			//Timer_Event(s_id, s_id, CL_END_MATCH, 3100ms);
		}

		break;

	}
	case CS_PACKET_UMB: {

		if (cl.bIsSnowman) break;
		if (cl.bHasUmbrella)
		{
			cs_packet_umb* packet = reinterpret_cast<cs_packet_umb*>(p);
			if (!packet->end)
				cout << "플레이어 " << cl._s_id << ": 우산 사용 " << endl;
			else
				cout << "플레이어 " << cl._s_id << ": 우산 접기" << endl;
			for (auto& other : clients) {
				if (ST_INGAME != other._state)
					continue;
				packet->type = SC_PACKET_UMB;
				other.do_send(sizeof(*packet), packet);
			}
		}

		break;

	}
	case CS_PACKET_CHAT: {
		cs_packet_chat* packet = reinterpret_cast<cs_packet_chat*>(p);
		int p_s_id = packet->s_id;
		switch (packet->cheat_type)
		{
		case CHEAT_HP_UP:
		{
			cout << "플레이어 " << cl._s_id << ": 치트 1 사용 " << endl;
			if (cl._hp + 30 > cl._max_hp)
				cl._hp = cl._max_hp;
			else
				cl._hp += 30;
			send_hp_packet(cl._s_id);
			break;
		}
		case CHEAT_HP_DOWN:
		{
			if (cl.bIsSnowman) break;
			cout << "플레이어 " << cl._s_id << "치트 2 사용 " << endl;
			int current_hp = cl._hp;
			cl._hp -= 30;
			if (cl._hp < 270) cl._hp = 270;
			send_hp_packet(cl._s_id);

			if (current_hp == cl._max_hp && cl.is_bone == true)
			{
				cl._is_active = true;
				player_heal(cl._s_id);
			}

			if (cl._hp <= cl._min_hp)
			{
				cl.iCurrentSnowballCount = 0;
				cl.iCurrentIceballCount = 0;
				cl.iCurrentMatchCount = 0;
				cl.iMaxSnowballCount = cl.iOriginMaxSnowballCount;
				cl.iMaxMatchCount = cl.iOriginMaxMatchCount;
				cl.bHasBag = false;
				cl.bHasUmbrella = false;
				cl.bIsSnowman = true;
				sc_packet_status_change _packet;
				_packet.size = sizeof(_packet);
				_packet.type = SC_PACKET_STATUS_CHANGE;
				_packet.state = ST_SNOWMAN;
				_packet.s_id = s_id;
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					other.do_send(sizeof(_packet), &_packet);
					//cout << "눈사람" << endl;
					//	cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;
				}
				int cnt = 0;
				int target_s_id;
				for (auto& other : clients) {
					if (cl._s_id == other._s_id) continue;
					if (ST_INGAME != other._state) continue;
					if (false == other.bIsSnowman)
					{
						cnt++;
						//cout << "눈덩이 cnt" << endl;

						target_s_id = other._s_id;
					}
				}
				if (cnt == 1) {
					for (auto& other : clients) {
						if (ST_INGAME != other._state)
							continue;
						send_game_end(target_s_id, other._s_id);
					}
					cout << "게임종료" << endl;
				}
			}

			break;
		}
		case CHEAT_SNOW_PLUS:
		{
			cout << "플레이어 " << cl._s_id << "치트 3 사용 " << endl;

			if (cl.iMaxSnowballCount >= cl.iCurrentSnowballCount + 5)
				cl.iCurrentSnowballCount += 5;
			else
				cl.iCurrentSnowballCount = cl.iMaxSnowballCount;
			cs_packet_get_item packet;
			packet.size = sizeof(packet);
			packet.type = SC_PACKET_GET_ITEM;
			packet.s_id = cl._s_id;
			packet.item_type = ITEM_SNOW;
			packet.destroy_obj_id = -1;
			packet.current_bullet = cl.iCurrentSnowballCount;
			for (auto& other : clients) {
				if (ST_INGAME != other._state) continue;
				other.do_send(sizeof(packet), &packet);
			}
			break;
		}
		case CHEAT_ICE_PLUS:
		{
			cout << "플레이어 " << cl._s_id << "치트 4 사용 " << endl;

			if (cl.iMaxIceballCount >= cl.iCurrentIceballCount + 5)
				cl.iCurrentIceballCount += 5;
			else
				cl.iCurrentIceballCount = cl.iMaxIceballCount;
			cs_packet_get_item packet;
			packet.size = sizeof(packet);
			packet.type = SC_PACKET_GET_ITEM;
			packet.s_id = cl._s_id;
			packet.item_type = ITEM_ICE;
			packet.destroy_obj_id = -1;
			packet.current_bullet = cl.iCurrentIceballCount;
			for (auto& other : clients) {
				if (ST_INGAME != other._state) continue;
				other.do_send(sizeof(packet), &packet);
			}
			break;
		}
		default:
			break;
		}

		break;

	}
	case CS_PACKET_GET_ITEM: {
		cs_packet_get_item* packet = reinterpret_cast<cs_packet_get_item*>(p);
		int p_s_id = packet->s_id;

		switch (packet->item_type)
		{

		case ITEM_BAG:
		{
			int item_num = packet->destroy_obj_id;
			bool get_item = is_item(item_num);
			if (!cl.bHasBag) {
				cl.iMaxSnowballCount += 5;	// 눈덩이 10 -> 15 까지 보유 가능
				cl.iMaxIceballCount += 5;	// 얼음 10 -> 15 까지 보유 가능
				cl.iMaxMatchCount += 1;	// 성냥 2 -> 3 까지 보유 가능
				cl.bHasBag = true;
			}
			if (get_item && cl.bHasBag == true) {
				cl.bHasBag = true;
				packet->type = SC_PACKET_GET_ITEM;
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					packet->type = SC_PACKET_GET_ITEM;
					other.do_send(sizeof(*packet), packet);
				}
				cout << "플레이어: [" << cl._s_id << "] 가방 파밍 성공" << endl;
			}
			else
				break;
				//cout << "플레이어: [" << cl._s_id << "] 가방 파밍 실패" << endl;

		break;
		}
		case ITEM_UMB:
		{
			int item_num = packet->destroy_obj_id;
			bool get_item = is_item(item_num);
			if (get_item && cl.bHasUmbrella == false) {
				cl.bHasUmbrella = true;
				packet->type = SC_PACKET_GET_ITEM;
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					packet->type = SC_PACKET_GET_ITEM;
					other.do_send(sizeof(*packet), packet);
				}
				cout << "플레이어: [" << cl._s_id << "] 우산 파밍 성공" << endl;
			}
			else
				break;
				//cout << "플레이어: [" << cl._s_id << "] 우산 파밍 실패" << endl;


		break;
		}
		case ITEM_JET:
		{
			packet->type = SC_PACKET_GET_ITEM;
			for (auto& other : clients) {
				if (ST_INGAME != other._state) continue;
				if (s_id == other._s_id) continue;
				packet->type = SC_PACKET_GET_ITEM;
				other.do_send(sizeof(*packet), packet);
			}
			cout << "플레이어: [" << cl._s_id << "] 제트스키 on/off" << endl;

			break;
		}
		case ITEM_MAT:
		{
			int item_num = packet->destroy_obj_id;
			bool get_item = is_item(item_num);
			if (get_item && cl.iMaxMatchCount > cl.iCurrentMatchCount) {
				cl.iCurrentMatchCount++;
				packet->type = SC_PACKET_GET_ITEM;
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					packet->type = SC_PACKET_GET_ITEM;
					other.do_send(sizeof(*packet), packet);
				}
				cout << "플레이어: [" << cl._s_id << "] 성냥 파밍 성공" << endl;
			}
			else
				break;
				//cout << "플레이어: [" << cl._s_id << "] 성냥 파밍 실패" << endl;


		break;
		}
		case ITEM_SNOW:
		{
			int snow_drift_num = packet->destroy_obj_id;
			bool get_snowball = is_snowdrift(snow_drift_num);
			if (get_snowball) {
				if (cl.iMaxSnowballCount >= cl.iCurrentSnowballCount + 5)
					cl.iCurrentSnowballCount += 5;
				else
					cl.iCurrentSnowballCount = cl.iMaxSnowballCount;

				packet->type = SC_PACKET_GET_ITEM;
				packet->current_bullet = cl.iCurrentSnowballCount;
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					packet->type = SC_PACKET_GET_ITEM;
					other.do_send(sizeof(*packet), packet);
				}
				cout << "플레이어: [" << cl._s_id << "] 눈무더기 파밍 성공" << endl;
			}
			else
				//cout << "플레이어: [" << cl._s_id << "]눈무더기 파밍 실패" << endl;
				break;
		break;
		}
		case ITEM_ICE:
		{
			int ice_drift_num = packet->destroy_obj_id;
			bool get_iceball = is_icedrift(ice_drift_num);
			if (get_iceball) {
				if (cl.iMaxIceballCount >= cl.iCurrentIceballCount + 5)
					cl.iCurrentIceballCount += 5;
				else
					cl.iCurrentIceballCount = cl.iMaxIceballCount;

				packet->type = SC_PACKET_GET_ITEM;
				packet->current_bullet = cl.iCurrentIceballCount;
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					packet->type = SC_PACKET_GET_ITEM;
					other.do_send(sizeof(*packet), packet);
				}

				cout << "플레이어: [" << cl._s_id << "] 얼음무더기 파밍 성공 현재 눈개수"  << cl.iCurrentIceballCount << endl;
			}
			else
				//cout << "플레이어: [" << cl._s_id << "]눈무더기 파밍 실패" << endl;
				break;
		}
		default:
			break;
		}
		//cout << "플레이어[" << s_id << "]가 " << "아이템 [" << _item_no << "]얻음" << endl;

		break;
	}
	case CS_PACKET_THROW_SNOW: {

		cs_packet_throw_snow* packet = reinterpret_cast<cs_packet_throw_snow*>(p);
		switch (packet->bullet)
		{
		case BULLET_SNOWBALL:
		{
			if (cl.iCurrentSnowballCount <= 0) break;

			cout << "throw BULLET_SNOWBALL " << packet->speed << endl;
			cl.iCurrentSnowballCount--;
			break;
		}
		case BULLET_ICEBALL:
		{
			if (cl.iCurrentIceballCount <= 0) break;

			cout << "throw BULLET_ICEBALL " << packet->speed << endl;
			cl.iCurrentIceballCount--;
			break;
		}
		default:
			break;
		}
		for (auto& other : clients) {
			if (ST_INGAME != other._state)
				continue;
			packet->type = SC_PACKET_THROW_SNOW;
			other.do_send(sizeof(*packet), packet);
		}
		break;

	}
	case CS_PACKET_CANCEL_SNOW: {

		cs_packet_cancel_snow* packet = reinterpret_cast<cs_packet_cancel_snow*>(p);
		for (auto& other : clients) {
			if (ST_INGAME != other._state)
				continue;
			packet->type = SC_PACKET_CANCEL_SNOW;
			other.do_send(sizeof(*packet), packet);
		}
		break;

	}
	case CS_PACKET_GUNFIRE: {
		if (cl.iCurrentSnowballCount < 4) break;
		printf("gunfire\n");
		cl.iCurrentSnowballCount -= 5;
		cs_packet_fire* packet = reinterpret_cast<cs_packet_fire*>(p);
		for (auto& other : clients) {
			if (ST_INGAME != other._state)
				continue;
			packet->type = SC_PACKET_GUNFIRE;
			rand_arr(packet->rand_int);
			other.do_send(sizeof(*packet), packet);
		}
		break;

	}
	case CS_PACKET_LOGOUT: {
		cs_packet_logout* packet = reinterpret_cast<cs_packet_logout*>(p);
		cout << "[Recv logout]";

		Disconnect(s_id);
		break;
	}
	case CS_PACKET_STATUS_CHANGE: {
		sc_packet_status_change* packet = reinterpret_cast<sc_packet_status_change*>(p);

		printf_s("[Recv status change] status : %d\n", packet->state);

		if (packet->state == ST_INBURN)
		{
			cout << s_id << "플레이어 모닥불 내부" << endl;

			if (cl.bIsSnowman) break;
			if (false == cl.is_bone) {
				cl.is_bone = true;
			}
			cl._is_active = true;
			player_heal(cl._s_id);

			cout << s_id << "플레이어 모닥불 내부" << endl;

		}
		else if (packet->state == ST_OUTBURN)
		{
			cout << s_id << "플레이어 모닥불 밖" << endl;
			if (cl.bIsSnowman) break;
			if (true == cl.is_bone) {
				cl.is_bone = false;

			}
			cl._is_active = true;
			player_damage(cl._s_id);
			cout << s_id << "플레이어 모닥불 밖" << endl;
		}
		else if (packet->state == ST_SNOWMAN)
		{
			//cout << "플레이어" << s_id <<"가 플레이어" <<packet->s_id << "가 눈사람이라 전송함" << endl;

			if (false == clients[packet->s_id].bIsSnowman) {
				clients[packet->s_id].bIsSnowman = true;
				clients[packet->s_id]._hp = clients[packet->s_id]._min_hp;
				//send_hp_packet(packet->s_id);
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					send_state_change(packet->s_id, other._s_id, ST_SNOWMAN);
					//cout << "눈사람" << endl;
					//	cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;
				}

				int cnt = 0;
				int target_s_id;
				for (auto& other : clients) {
					//if (cl._s_id == other._s_id) continue;
					if (ST_INGAME != other._state) continue;
					if (false == other.bIsSnowman)
					{
						cnt++;
						//cout << "충돌 cnt" << endl;

						target_s_id = other._s_id;
					}
				}
				if (cnt == 1) {
					for (auto& other : clients) {
						if (ST_INGAME != other._state)
							continue;
						send_game_end(target_s_id, other._s_id);
					}
					cout << "게임종료" << endl;
				}

			}
			//cout << "플레이어" << packet->s_id <<" 눈사람" << endl;

		}

		else if (packet->state == ST_ANIMAL)
		{
			//cout << "플레이어" << s_id << "가 플레이어" << packet->s_id << "가 동물이라 전송함" << endl;
			if (true == clients[packet->s_id].bIsSnowman) {
				clients[packet->s_id]._hp = clients[packet->s_id]._BeginSlowHP;
				clients[packet->s_id].bIsSnowman = false;
				//send_hp_packet(packet->s_id);
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					send_state_change(packet->s_id, other._s_id, ST_ANIMAL);
					//cout << "동물화" << endl;
					//	cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;
				}
			}
			//cout << "플레이어" << packet->s_id << " 동물" << endl;



		}

		break;
	}
	case CS_PACKET_READY: {
		sc_packet_ready _packet;
		_packet.size = sizeof(_packet);
		_packet.type = SC_PACKET_READY;
		_packet.s_id = s_id;
		cl.b_ready = true;
		for (auto& other : clients) {
			if (s_id == other._s_id)
				continue;
			if (ST_INGAME != other._state)
				continue;
			other.do_send(sizeof(_packet), &_packet);
		}
		cout << s_id << "플레이어 레디" << endl;

		for (auto& other : clients) {
			if (s_id == other._s_id)
				continue;
			if (ST_INGAME != other._state)
				continue;
			if (other.b_ready == false)
				return;
		}

		sc_packet_start s_packet;
		s_packet.size = sizeof(s_packet);
		s_packet.type = SC_PACKET_START;

		for (auto& other : clients) {
			if (ST_INGAME != other._state)
				continue;
			other.do_send(sizeof(s_packet), &s_packet);
		}

		SetEvent(g_timer);
		GA.g_start_game = true;

		cout << "게임 스타트" << endl;
		break;
	}
	case CS_PACKET_OPEN_BOX: {
		if (cl.bIsSnowman) break;
		printf("OPEN_BOX\n");
		cs_packet_open_box* packet = reinterpret_cast<cs_packet_open_box*>(p);
		for (auto& other : clients) {
			if (s_id == other._s_id) continue;
			if (ST_INGAME != other._state) continue;
			packet->type = SC_PACKET_OPEN_BOX;
			other.do_send(sizeof(*packet), packet);
		}
		break;
	}

	default:
		cout << " 오류패킷타입" << packet_type << endl;
		printf("Unknown PACKET type\n");
		break;
	}
}

//워크 쓰레드
void worker_thread()
{
	while (1) {
		DWORD num_byte;
		LONG64 iocp_key;
		WSAOVERLAPPED* p_over;
		BOOL ret = GetQueuedCompletionStatus(g_h_iocp, &num_byte, (PULONG_PTR)&iocp_key, &p_over, INFINITE);

		int _s_id = static_cast<int>(iocp_key);
		Overlap* exp_over = reinterpret_cast<Overlap*>(p_over);
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			cout << "GQCS Error : ";
			error_display(err_no);
			cout << endl;
			Disconnect(_s_id);
			if (exp_over->_op == OP_SEND)
				delete exp_over;
			continue;
		}

		switch (exp_over->_op) {
		case OP_RECV: {
			if (num_byte == 0) {
				cout << "연결종료" << endl;
				Disconnect(_s_id);
				continue;
			}
			CLIENT& cl = clients[_s_id];
			int remain_data = num_byte + cl._prev_size;
			unsigned char* packet_start = exp_over->_net_buf;
			int packet_size = packet_start[0];

			while (packet_size <= remain_data) {
				process_packet(_s_id, packet_start);
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
			break;
		}
		case OP_SEND: {
			if (num_byte != exp_over->_wsa_buf.len) {
				cout << "send 에러" << endl;
				Disconnect(_s_id);
			}
			delete exp_over;
			break;
		}
		case OP_ACCEPT: {
			cout << "Accept Completed.\n";
			SOCKET c_socket = *(reinterpret_cast<SOCKET*>(exp_over->_net_buf));


			int n__s_id = get_id();
			if (-1 == n__s_id) {
				cout << "user over.\n";
			}
			else {
				CLIENT& cl = clients[n__s_id];
				cl.state_lock.lock();
				cl._s_id = n__s_id;
				cl._state = ST_ACCEPT;
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
				sizeof(SOCKADDR_IN) + 16, NULL, &exp_over->_wsa_over);
		}
					  break;
		case OP_PLAYER_HEAL: {
			if (clients[_s_id]._hp + 10 <= clients[_s_id]._max_hp) {
				clients[_s_id]._hp += 10;
				player_heal(_s_id);
			}
			else
				clients[_s_id]._hp = clients[_s_id]._max_hp;
			send_hp_packet(_s_id);

			delete exp_over;
			break;
		}
		case OP_PLAYER_DAMAGE: {
			if (clients[_s_id].bIsSnowman) break;
			if (clients[_s_id].is_bone == false) {
				if (clients[_s_id]._hp - 1 > clients[_s_id]._min_hp) {
					clients[_s_id]._hp -= 1;
					player_damage(clients[_s_id]._s_id);
					send_hp_packet(_s_id);
				}
				else if (clients[_s_id]._hp - 1 == clients[_s_id]._min_hp)
				{
					//cout << "모닥불 데미지 눈사람" << endl;
					clients[_s_id].iCurrentSnowballCount = 0;
					clients[_s_id].iCurrentIceballCount = 0;
					clients[_s_id].iCurrentMatchCount = 0;
					clients[_s_id].iMaxSnowballCount = clients[_s_id].iOriginMaxSnowballCount;
					clients[_s_id].iMaxIceballCount = clients[_s_id].iOriginMaxIceballCount;
					clients[_s_id].iMaxMatchCount = clients[_s_id].iOriginMaxMatchCount;
					clients[_s_id].bHasBag = false;
					clients[_s_id].bHasUmbrella = false;
					clients[_s_id].bIsSnowman = true;
					clients[_s_id]._hp -= 1;
					send_hp_packet(_s_id);
					for (auto& other : clients) {
						if (ST_INGAME != other._state) continue;
						send_state_change(_s_id, other._s_id, ST_SNOWMAN);
					}
					int cnt = 0;
					int target_s_id;
					for (auto& other : clients) {
						if (_s_id == other._s_id) continue;
						if (ST_INGAME != other._state) continue;
						if (false == other.bIsSnowman)
						{
							cnt++;
							target_s_id = other._s_id;
						}
					}
					if (cnt == 1) {
						for (auto& other : clients) {
							if (ST_INGAME != other._state)
								continue;
							send_game_end(target_s_id, other._s_id);
						}
						cout << "게임종료" << endl;
					}
				}
			}
			delete exp_over;
			break;
		}
		}


	}

}

//타이머
void ev_timer()
{
	WaitForSingleObject(g_timer, INFINITE);
	{
		timer_q.Clear();
	}
	while (true) {
		timer_ev order;
		timer_q.WaitPop(order);
		//auto t = order.start_t - chrono::system_clock::now();
		int s_id = order.this_id;
		if (false == is_player(s_id)) continue;
		if (clients[s_id]._state != ST_INGAME) continue;
		if (clients[s_id]._is_active == false) continue;
		if (order.start_t <= chrono::system_clock::now()) {
			if (order.order == CL_BONEFIRE) {
				if (clients[s_id].is_bone == false) continue;
				Player_Event(s_id, order.target_id, OP_PLAYER_HEAL);
				this_thread::sleep_for(50ms);
			}
			else if (order.order == CL_BONEOUT) {
				if (clients[s_id].is_bone == true) continue;
				Player_Event(s_id, order.target_id, OP_PLAYER_DAMAGE);
				this_thread::sleep_for(50ms);
			}
			else if (order.order == CL_MATCH) {
				Player_Event(s_id, order.target_id, OP_PLAYER_HEAL);
				this_thread::sleep_for(50ms);
			}
			else if (order.order == CL_END_MATCH) {
				send_is_bone_packet(s_id);
				this_thread::sleep_for(50ms);
			}
		}
		else {
			timer_q.Push(order);
			this_thread::sleep_for(10ms);
		}
	}

}

//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------

