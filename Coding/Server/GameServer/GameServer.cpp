//----------------------------------------------------------------------------------------------------------------------------------------------
// GameServer.cpp 파일
//----------------------------------------------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "CorePch.h"
#include "CLIENT.h"
#include "Overlap.h"
#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

#include <thread>
#include <cmath>

HANDLE g_h_iocp;
HANDLE g_timer;
mutex  g_snow_mutex;
mutex  g_item_mutex;

condition_variable cv;
SOCKET sever_socket;
LockQueue<timer_ev> timer_q;

//concurrency::concurrent_priority_queue <timer_ev> timer_q;
array <CLIENT, MAX_USER> clients;
bool g_snow_drift[MAX_SNOWDRIFT] = {};
bool g_item[MAX_ITEM] = {};

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;


void show_err();
int get_id();
void send_login_ok_packet(int _s_id);
void send_login_fail_packet(int _s_id);
void send_move_packet(int _id, int target);
void send_remove_object(int _s_id, int victim);
void send_put_object(int _s_id, int target);
void send_chat_packet(int user_id, int my_id, char* mess);
void send_status_packet(int _s_id);
void Disconnect(int _s_id);
void Player_Event(int target, int player_id, COMMAND type);
void Timer_Event(int np_s_id, int user_id, EVENT_TYPE ev, std::chrono::milliseconds ms);
void process_packet(int _s_id, unsigned char* p);
void worker_thread();
void ev_timer();
void send_state_change(int s_id, int target, STATE_Type stat);
bool is_snowdrift(int obj_id);


//이동
void send_hp_packet(int _id)
{
	sc_packet_hp_change packet;
	packet.s_id = _id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_HP;
	packet.hp = clients[_id]._hp;

	// printf_s("[Send hp change] id : %d, hp : %d\n", packet.s_id, packet.hp);
	clients[_id].do_send(sizeof(packet), &packet);
}

//모닥불 확인
void send_is_bone_packet(int _id)
{
	cout << "모닥불 확인" << endl;
	sc_packet_is_bone packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_IS_BONE;
	// printf_s("[Send hp change] id : %d, hp : %d\n", packet.s_id, packet.hp);
	clients[_id].do_send(sizeof(packet), &packet);
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
	if (false == clients[s_id].bIsSnowman) {
		if (clients[s_id]._hp > clients[s_id]._min_hp) {
			Timer_Event(s_id, s_id, CL_BONEOUT, 1000ms);;
		}
	}
}

//플레이어 판별
bool is_player(int id)
{
	return (id >= 0) && (id < MAX_USER);
}

//모닥불 범위 판정
bool is_bonfire(int a)
{
	if (BONFIRE_RANGE < abs(clients[a].x)) return false;
	if (BONFIRE_RANGE < abs(clients[a].y)) return false;
	return true;
}

//근처 객체 판별
bool is_near(int a, int b)
{
	if (RANGE < abs(clients[a].x - clients[b].x)) return false;
	if (RANGE < abs(clients[a].y - clients[b].y)) return false;
	return true;
}

bool is_snowdrift(int obj_id)
{
	unique_lock<mutex> _lock(g_snow_mutex);
	if (g_snow_drift[obj_id]) {
		g_snow_drift[obj_id] = false;
		return true;
	}
	return false;
}

bool is_item(int obj_id)
{
	unique_lock<mutex> _lock(g_item_mutex);
	if (g_item[obj_id]) {
		g_item[obj_id] = false;
		return true;
	}
	return false;
}



int main()
{
	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
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
	
	for (int i = 0; i < MAX_SNOWDRIFT; ++i)
		g_snow_drift[i] = true;

	for (int i = 0; i < MAX_ITEM; ++i)
		g_item[i] = true;

	for (int i = 0; i < MAX_USER; ++i)
		clients[i]._s_id = i;

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
	//timer_thread.join();
	for (auto& cl : clients) {
		if (ST_INGAME == cl._state)
			Disconnect(cl._s_id);
	}
	closesocket(sever_socket);
	WSACleanup();
}

void show_err() {
	cout << "error" << endl;
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

//로그인 허용
void send_login_ok_packet(int _s_id)
{
	sc_packet_login_ok packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;
	packet.s_id = _s_id;
	packet.x = clients[_s_id].x;
	packet.y = clients[_s_id].y;
	packet.z = clients[_s_id].z;
	packet.yaw = clients[_s_id].Yaw;
	//packet.Yaw = clients[_s_id].Yaw;
	//packet.Pitch = clients[_s_id].Pitch;
	//packet.Roll = clients[_s_id].Roll;*/
	printf("[Send login ok] id : %d, location : (%f,%f,%f) yaw : %f\n", _s_id, packet.x, packet.y, packet.z, packet.yaw);
	clients[_s_id].do_send(sizeof(packet), &packet);
}

//로그인 실패
void send_login_fail_packet(int _s_id)
{
	sc_packet_login_fail packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_FAIL;
	clients[_s_id].do_send(sizeof(packet), &packet);
}

//오브젝트 제거
void send_remove_object(int _s_id, int victim)
{
	sc_packet_remove_object packet;
	packet.s_id = victim;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_REMOVE_OBJECT;
	clients[_s_id].do_send(sizeof(packet), &packet);
}

//오브젝트 생성
void send_put_object(int _s_id, int target)
{
	sc_packet_put_object packet;
	packet.s_id = target;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_PUT_OBJECT;
	packet.x = clients[target].x;
	packet.y = clients[target].y;
	packet.z = clients[target].z;

	strcpy_s(packet.name, clients[target].name);
	packet.object_type = 0;
	clients[_s_id].do_send(sizeof(packet), &packet);
}

//메세지 전송
void send_chat_packet(int user_id, int my_id, char* mess)
{
	sc_packet_chat packet;
	packet.id = my_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_CHAT;
	strcpy_s(packet.message, mess);
	clients[user_id].do_send(sizeof(packet), &packet);
}

//플레이어 데이터 변경
void send_status_packet(int _s_id)
{
	sc_packet_status_change packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_STATUS_CHANGE;
	clients[_s_id].do_send(sizeof(packet), &packet);
}

//해제
void Disconnect(int _s_id)
{
	CLIENT& cl = clients[_s_id];
	cl.vl.lock();
	unordered_set <int> my_vl = cl.viewlist;
	cl.vl.unlock();

	strcpy_s(clients[_s_id]._id, " ");
	for (auto& other : my_vl) {
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
	}
	//clients[_s_id].state_lock.lock();
	clients[_s_id]._state = ST_FREE;
	//clients[_s_id].state_lock.unlock();
	closesocket(clients[_s_id]._socket);
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
void Timer_Event(int np_s_id, int user_id, EVENT_TYPE ev, std::chrono::milliseconds ms)
{
	timer_ev order;
	order.this_id = np_s_id;
	order.target_id = user_id;
	order.order = ev;
	order.start_t = chrono::system_clock::now() + ms;
	timer_q.Push(order);
}


void send_state_change(int s_id, int target, STATE_Type stat)
{
	sc_packet_status_change _packet;
	_packet.size = sizeof(_packet);
	_packet.type = SC_PACKET_STATUS_CHANGE;
	_packet.state = stat;
	_packet.s_id = s_id;
	clients[target].do_send(sizeof(_packet), &_packet);
}

//패킷 판별
void process_packet(int s_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	CLIENT& cl = clients[s_id];

	switch (packet_type) {
	case CS_PACKET_LOGIN: {
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p);

		CLIENT& cl = clients[s_id];
		printf_s("[Recv login] ID : %s, PASSWORD : %s, z : %f\n", packet->id, packet->pw, packet->z);

		/*	for (int i = 0; i < MAX_USER; ++i) {
				clients[i].state_lock.lock();
				if (ST_INGAME == clients[i]._state) {
					if (strcmp(packet->id, clients[i]._id) == 0) {
						send_login_fail_packet(s_id);
						cout << packet->id << "접속중인 플레이어" << endl;
						clients[i].state_lock.unlock();
						return;
					}
				}
				clients[i].state_lock.unlock();
			}*/
		cl.state_lock.lock();
		cl._state = ST_INGAME;
		cl.state_lock.unlock();
		cl.x = 300.0f * cos(s_id + 45.0f);
		cl.y = 300.0f * sin(s_id + 45.0f);
		cl.z = packet->z;
		cl.Yaw = s_id * 55.0f - 115.0f;
		if (cl.Yaw > 180) cl.Yaw -= 360;

		cl._hp = cl._max_hp;

		//sc_packet_login_ok _packet;
		//_packet.size = sizeof(_packet);
		//_packet.type = SC_PACKET_LOGIN_OK;
		//_packet.s_id = s_id;
		/*packet.x = clients[_s_id].x;
		packet.y = clients[_s_id].y;
		packet.z = clients[_s_id].z;
		packet.Yaw = clients[_s_id].Yaw;
		packet.Pitch = clients[_s_id].Pitch;
		packet.Roll = clients[_s_id].Roll;*/
		//cout << "로그인 허용 전송" << s_id << endl;
		//cl.do_send(sizeof(_packet), &_packet);
		send_login_ok_packet(s_id);
		//cout << "플레이어[" << s_id << "]" << " 로그인 성공" << endl;

		// 새로 접속한 플레이어의 정보를 주위 플레이어에게 보낸다
		for (auto& other : clients) {
			if (other._s_id == s_id) continue;
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
			sc_packet_put_object packet;
			packet.s_id = cl._s_id;
			strcpy_s(packet.name, cl.name);
			packet.object_type = 0;
			packet.size = sizeof(packet);
			packet.type = SC_PACKET_PUT_OBJECT;
			packet.x = cl.x;
			packet.y = cl.y;
			packet.z = cl.z;
			packet.yaw = cl.Yaw;

			//printf_s("[Send put object] id : %d, location : (%f,%f,%f), yaw : %f\n", packet.s_id, packet.x, packet.y, packet.z, packet.yaw);
			//cout << other._s_id << "에게 " << cl._s_id << "을 " << endl;
			other.do_send(sizeof(packet), &packet);
		}

		// 새로 접속한 플레이어에게 주위 객체 정보를 보낸다
		for (auto& other : clients) {
			if (other._s_id == s_id) continue;
			other.state_lock.lock();
			if (ST_INGAME != other._state) {
				other.state_lock.unlock();
				continue;
			}
			else other.state_lock.unlock();

			//if (false == is_near(other._s_id, s_id))
				//continue;

			//clients[s_id].vl.lock();
			//clients[s_id].viewlist.insert(other._id);
			//clients[s_id].vl.unlock();

			sc_packet_put_object packet;
			packet.s_id = other._s_id;
			strcpy_s(packet.name, other.name);
			packet.object_type = 0;
			packet.size = sizeof(packet);
			packet.type = SC_PACKET_PUT_OBJECT;
			packet.x = other.x;
			packet.y = other.y;
			packet.z = other.z;
			packet.yaw = other.Yaw;

			//printf_s("[Send put object] id : %d, location : (%f,%f,%f), yaw : %f\n", packet.s_id, packet.x, packet.y, packet.z, packet.yaw);
			cl.do_send(sizeof(packet), &packet);
		}

		break;
	}
	case CS_PACKET_MOVE: {

		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(p);
		CLIENT& cl = clients[packet->sessionID];
		cl.x = packet->x;
		cl.y = packet->y;
		cl.z = packet->z;
		cl.Yaw = packet->yaw;
		cl.VX = packet->vx;
		cl.VY = packet->vy;
		cl.VZ = packet->vz;
		cl.direction = packet->direction;
		//printf_s("[Recv move] id : %d, location : (%f,%f,%f), yaw : %f,  v : (%f,%f,%f), dir : %f\n", packet->sessionID, cl.x, cl.y, cl.z, cl.Yaw, cl.VX, cl.VY, cl.VZ, cl.direction);

		//auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		//cout << millisec_since_epoch - packet->move_time << "ms" << endl;

		//send_status_packet(s_id);

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
			//	cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;

		}

		//새로 시야에 들어온 플레이어
		//for (auto other_id : near_list) {
		//	if (0 == my_vl.count(other_id)) {
		//		cl.vl.lock();
		//		cl.viewlist.insert(other_id);
		//		cl.vl.unlock();
		//		send_put_object(cl._s_id, other_id);


		//		clients[other_id].vl.lock();
		//		if (0 == clients[other_id].viewlist.count(cl._s_id)) {
		//			clients[other_id].viewlist.insert(cl._s_id);
		//			clients[other_id].vl.unlock();
		//			send_put_object(other_id, cl._s_id);
		//		}
		//		else {
		//			clients[other_id].vl.unlock();
		//			send_move_packet(other_id, cl._s_id);
		//		}
		//	}
		//	//시야에 존재하는 플레이어
		//	else {
		//		clients[other_id].vl.lock();
		//		if (0 != clients[other_id].viewlist.count(cl._s_id)) {
		//			clients[other_id].vl.unlock();
		//			send_move_packet(other_id, cl._s_id);
		//		}
		//		else {
		//			clients[other_id].viewlist.insert(cl._s_id);
		//			clients[other_id].vl.unlock();
		//			send_put_object(other_id, cl._s_id);
		//		}
		//	}
		//}
		//// 시야에서 벗어난 플레이어 
		//for (auto other_id : my_vl) {
		//	if (0 == near_list.count(other_id)) {
		//		cl.vl.lock();
		//		cl.viewlist.erase(other_id);
		//		cl.vl.unlock();
		//		send_remove_object(cl._s_id, other_id);
		//		clients[other_id].vl.lock();
		//		if (0 != clients[other_id].viewlist.count(cl._s_id)) {
		//			clients[other_id].viewlist.erase(cl._s_id);
		//			clients[other_id].vl.unlock();
		//			send_remove_object(other_id, cl._s_id);
		//		}
		//		else clients[other_id].vl.unlock();
		//	}
		//}


		//printf("Move\n");
		break;
	}
	case CS_PACKET_ATTACK: {
		cs_packet_attack* packet = reinterpret_cast<cs_packet_attack*>(p);
		for (auto& other : clients) {
			if (other._s_id == s_id)
				continue;
			if (ST_INGAME != other._state)
				continue;
			packet->type = SC_PACKET_ATTACK;
			//cout << "플레이어[" << packet->s_id << "]가" << "플레이어[" << other._s_id << "]에게 보냄" << endl;
			other.do_send(sizeof(*packet), packet);
			//cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;
		}
		printf("attack\n");
		break;

	}
	case CS_PACKET_DAMAGE: {
		if (cl.bIsSnowman) break;
		cout << "플레이어 " << cl._s_id << "데미지 받음 " << endl;
		cl._hp -= 10;
		if (cl._hp < 270) cl._hp = 270;
		send_hp_packet(cl._s_id);

		if (cl._hp <= cl._min_hp)
		{
			cl.iCurrentSnowballCount = 0;
			sc_packet_status_change _packet;
			_packet.size = sizeof(_packet);
			_packet.type = SC_PACKET_STATUS_CHANGE;
			_packet.state = ST_SNOWMAN;
			_packet.s_id = s_id;
			for (auto& other : clients) {
				if (ST_INGAME != other._state)
					continue;
				other.do_send(sizeof(_packet), &_packet);
				cout << "눈사람" << endl;
				//	cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;
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
			cl.is_bone = true;
			Timer_Event(s_id, s_id, CL_MATCH, 1000ms);
			Timer_Event(s_id, s_id, CL_MATCH, 2000ms);
			Timer_Event(s_id, s_id, CL_MATCH, 3000ms);
			Timer_Event(s_id, s_id, CL_END_MATCH, 3100ms);
		}

		break;

	}
	case CS_PACKET_CHAT: {
		cs_packet_chat* packet = reinterpret_cast<cs_packet_chat*>(p);
		int p_s_id = packet->s_id;
		float x = packet->x;
		float y = packet->y;
		float z = packet->z;
		//cout << "플레이어[" << s_id << "]가  받음" << "[" << p_s_id << "] " << x << " " << y << " " << z << endl;
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
			if (get_item && cl.bHasBag ==false) {
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
				cout << "플레이어: [" << cl._s_id << "] 가방 파밍 실패" << endl;
			
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
				cout << "플레이어: [" << cl._s_id << "] 우산 파밍 실패" << endl;

			
			break;
		}
		case ITEM_MAT:
		{
			int item_num = packet->destroy_obj_id;
			bool get_item = is_item(item_num);
			if (get_item && cl.iOriginMaxMatchCount > cl.iCurrentMatchCount) {
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
				cout << "플레이어: [" << cl._s_id << "] 성냥 파밍 실패" << endl;

			
			break;
		}
		case ITEM_SNOW:
		{
			int snow_drift_num = packet->destroy_obj_id;
			bool get_snowball = is_snowdrift(snow_drift_num);
			if (get_snowball && cl.iMaxSnowballCount > cl.iCurrentSnowballCount) {
				cl.iCurrentSnowballCount++;
				packet->type = SC_PACKET_GET_ITEM;
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					packet->type = SC_PACKET_GET_ITEM;
					other.do_send(sizeof(*packet), packet);
				}
				cout <<"플레이어: ["<< cl._s_id << "] 눈무더기 파밍 성공" << endl;
			}
			else
				cout << "플레이어: [" << cl._s_id << "]눈무더기 파밍 실패" << endl;
			break;
		}
		default:
			break;
		}
		//cout << "플레이어[" << s_id << "]가 " << "아이템 [" << _item_no << "]얻음" << endl;
		
		break;
	}
	case CS_PACKET_THROW_SNOW: {
		if (cl.iCurrentSnowballCount <= 0) break;
		printf("attack\n");
		cl.iCurrentSnowballCount--;
		cs_packet_throw_snow* packet = reinterpret_cast<cs_packet_throw_snow*>(p);
		for (auto& other : clients) {
			if (ST_INGAME != other._state)
				continue;
			packet->type = SC_PACKET_THROW_SNOW;
			//cout << "플레이어[" << packet->s_id << "]가" << "플레이어[" << other._s_id << "]에게 보냄" << endl;

			//printf_s("[Send throw snow]\n");
			other.do_send(sizeof(*packet), packet);
			//cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;

		}


		break;

	}
	case CS_PACKET_LOGOUT: {
		cs_packet_logout* packet = reinterpret_cast<cs_packet_logout*>(p);
		cout << "[Recv logout]";
		for (auto& other : clients) {
			if (s_id == other._s_id) continue;
			if (ST_INGAME != other._state) continue;
			send_remove_object(other._s_id, s_id);
		}
		Disconnect(s_id);
		break;
	}
	case CS_PACKET_STATUS_CHANGE: {
		sc_packet_status_change* packet = reinterpret_cast<sc_packet_status_change*>(p);

		//printf_s("[Recv status change] status : %d\n", packet->state);

		if (packet->state == ST_INBURN)
		{
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
			cout << "플레이어" << s_id <<"가 플레이어" <<packet->s_id << "가 눈사람이라 전송함" << endl;

			if (false == clients[packet->s_id].bIsSnowman) {
				clients[packet->s_id].bIsSnowman = true;
				clients[packet->s_id]._hp = clients[packet->s_id]._min_hp;
				//send_hp_packet(packet->s_id);
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					send_state_change(packet->s_id, other._s_id, ST_SNOWMAN);
					cout << "눈사람" << endl;
					//	cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;
				}
			}
			cout << "플레이어" << packet->s_id <<" 눈사람" << endl;

		}

		else if (packet->state == ST_ANIMAL)
		{
			cout << "플레이어" << s_id << "가 플레이어" << packet->s_id << "가 동물이라 전송함" << endl;
			if (true == clients[packet->s_id].bIsSnowman) {
				clients[packet->s_id]._hp = clients[packet->s_id]._BeginSlowHP;
				clients[packet->s_id].bIsSnowman = false;
				//send_hp_packet(packet->s_id);
				for (auto& other : clients) {
					if (ST_INGAME != other._state)
						continue;
					send_state_change(packet->s_id, other._s_id, ST_ANIMAL);
					cout << "동물화" << endl;
					//	cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;
				}
			}
			cout << "플레이어" << packet->s_id << " 동물" << endl;



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
		cout << "게임 스타트" << endl;
		break;
	}
	default:
		printf("Unknown PACKET type\n");

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
				cl.x = rand() % ReZone_WIDTH;
				cl.y = rand() % ReZone_HEIGHT;
				cl.z = 0;
				cl.Yaw = 0;
				cl.Pitch = 0;
				cl.Roll = 0;
				//cl.x = 200;
				//cl.y = 200;
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
					player_damage(_s_id);
					send_hp_packet(_s_id);
					//cout << "hp -1" << endl;

				}
				else if (clients[_s_id]._hp - 1 == clients[_s_id]._min_hp) 
				{
					clients[_s_id].bIsSnowman = true;
					clients[_s_id]._hp -= 1;
					send_hp_packet(_s_id);
					clients[_s_id].iCurrentSnowballCount = 0;
					for (auto& other : clients) {
						if (ST_INGAME != other._state) continue;
						send_state_change(_s_id, other._s_id, ST_SNOWMAN);
						cout << "눈사람" << endl;
						
					}

				}
			}

			delete exp_over;
			break;
		}
		}


	}

}

//이동
void send_move_packet(int _id, int target)
{
	cs_packet_move packet;
	packet.sessionID = target;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_MOVE;
	packet.x = clients[target].x;
	packet.y = clients[target].y;
	packet.z = clients[target].z;
	packet.yaw = clients[target].Yaw;
	packet.vx = clients[target].VX;
	packet.vy = clients[target].VY;
	packet.vz = clients[target].VZ;
	packet.direction = clients[target].direction;
	//packet.move_time = clients[target].last_move_time;

	//printf_s("[Send move] id : %d, location : (%f,%f,%f), yaw : %f,  v : (%f,%f,%f)\n", packet.sessionID, packet.x, packet.y, packet.z, packet.yaw, packet.vx, packet.vy, packet.vz);
	clients[_id].do_send(sizeof(packet), &packet);
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

