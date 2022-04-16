//----------------------------------------------------------------------------------------------------------------------------------------------
// GameServer.cpp 파일
//----------------------------------------------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "CorePch.h"
#include "CLIENT.h"
#include "Overlap.h"
#include <thread>

#include <atomic>

HANDLE g_h_iocp;
SOCKET sever_socket;
concurrency::concurrent_priority_queue <timer_ev> timer_q;
array <CLIENT, MAX_USER> clients;

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

//이동
void send_hp_packet(int _id, int target)
{
	sc_packet_hp_change packet;
	packet.target = target;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_HP;
	packet.hp = clients[target]._hp;
	clients[_id].do_send(sizeof(packet), &packet);
}

void player_heal(int s_id)
{
	Timer_Event(s_id, s_id, CL_BONEFIRE, 1000ms);
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
	//packet.Yaw = clients[_s_id].Yaw;
	//packet.Pitch = clients[_s_id].Pitch;
	//packet.Roll = clients[_s_id].Roll;*/
	cout << "로그인 허용 전송" << "[" << _s_id <<"]" << " " << packet.x << " " << packet.y << " " << packet.z << endl;
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
	packet.id = victim;
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
	clients[_s_id].state_lock.lock();
	clients[_s_id]._state = ST_FREE;
	clients[_s_id].state_lock.unlock();
	closesocket(clients[_s_id]._socket);
	cout << "------------연결 종료------------" << endl;
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
	timer_q.push(order);
}

//패킷 판별
void process_packet(int s_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	CLIENT& cl = clients[s_id];

	switch (packet_type) {
	case CS_PACKET_LOGIN: {
		printf("login\n");
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p);

		CLIENT& cl = clients[s_id];
		printf_s("[INFO] 로그인 시도 {%s}/{%s}\n", packet->id, packet->pw);

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
		cl.x = 0 + (s_id*300);
		cl.y = 0 +(s_id * 300);
		cl.z = packet->z;


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
			packet.s_id = s_id;
			strcpy_s(packet.name, cl.name);
			packet.object_type = 0;
			packet.size = sizeof(packet);
			packet.type = SC_PACKET_PUT_OBJECT;
			packet.x = cl.x;
			packet.y = cl.y;
			packet.z = cl.z;

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
			cout << cl._s_id << "에게 " << other._s_id << "을 " << endl;
			cl.do_send(sizeof(packet), &packet);
		}

	   break;
	}
	case CS_PACKET_MOVE: {
		
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(p);
		CLIENT& cl = clients[s_id];
		cl.x = packet->x;
		cl.y = packet->y;
		cl.z = packet->z;
		cl.Yaw = packet->yaw;
		cl.Pitch = packet->pitch;
		cl.Roll = packet->roll;
		cl.VX = packet->vz;
		cl.VY = packet->vz;
		cl.VZ = packet->vz;
		//cout <<"플레이어["<< packet->sessionID<<"]" << "  x:" << packet->x << " y:" << packet->y << " z:" << packet->z << endl;
		//클라 recv 확인용

		//auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	    //cout << millisec_since_epoch - packet->move_time << "ms" << endl;

		send_status_packet(s_id);
		
		if (true == is_bonfire(cl._s_id))
		{
			//cout << "모닥불 지역" << endl;
			if (false == cl.is_bone) {
				cl.is_bone = true;
				cl._is_active = true;
				player_heal(cl._s_id);

			}
		}
		else
		{
			cl.is_bone = false;
			cl._is_active = false;
		}

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
			send_move_packet( other._s_id, cl._s_id);
			cout <<"움직인 플레이어" << cl._s_id << "보낼 플레이어" << other._s_id << endl;
					
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


		printf("Move\n");
		break;
	}
	case SC_PACKET_STATUS_CHANGE: {
		//printf("status\n");
		//printf("클라이언트 recv 성공\n");

		break;

	}
	case CS_PACKET_ATTACK: {
		printf("attack\n");
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
		
			if (clients[_s_id].is_bone == true) { 
				clients[_s_id]._hp += 1;
				player_heal(_s_id); 
			}
			send_hp_packet(_s_id, _s_id);
			//cout << "hp +1" << endl;
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
	packet.pitch = clients[target].Pitch;
	packet.roll = clients[target].Roll;
	packet.vx = clients[target].VX;
	packet.vy = clients[target].VY;
	packet.vz = clients[target].VZ;
	//packet.move_time = clients[target].last_move_time;
	clients[_id].do_send(sizeof(packet), &packet);
}
//타이머
void ev_timer() 
{

	while (true) {
		timer_ev order;
		timer_q.try_pop(order);
		//auto t = order.start_t - chrono::system_clock::now();
		int s_id = order.this_id;
		if (false == is_player(s_id)) continue;
		if (clients[s_id]._state != ST_INGAME) continue;
		if (clients[s_id]._is_active == false) continue;
		if (order.start_t <= chrono::system_clock::now()) {
			if (order.order == CL_BONEFIRE) {
				Player_Event(s_id, order.target_id, OP_PLAYER_HEAL);
				this_thread::sleep_for(50ms);
			}

		}
		else {
			timer_q.push(order);
			this_thread::sleep_for(10ms);

		}


	}

}

//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------

