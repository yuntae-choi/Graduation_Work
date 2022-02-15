//----------------------------------------------------------------------------------------------------------------------------------------------
//2021/12/08
// 2017182043 최윤태
// 겜서버 텀프
//----------------------------------------------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "CorePch.h"
#include "CLIENT.h"
#include "Overlap.h"
#include "Enum.h"



#define UNICODE  // 우리나라는 Default니깐 굳이 안써도 됨
#define NAME_LEN 20  
#define PHONE_LEN 60


HANDLE g_h_iocp;
SOCKET sever_socket;

concurrency::concurrent_priority_queue <timer_ev> timer_q;
array <CLIENT, MAX_USER + MAX_OBJ> clients;


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
void send_login_ok_packet(int c_id)
{
	sc_packet_login_ok packet;
	packet.id = c_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;
	packet.x = clients[c_id].x;
	packet.y = clients[c_id].y;
	packet.maxhp = clients[c_id]._max_hp;
	packet.hp = clients[c_id]._hp;
	packet.level = clients[c_id]._level;
	packet.exp = clients[c_id]._exp;
	clients[c_id].do_send(sizeof(packet), &packet);
}

//로그인 실패
void send_login_fail_packet(int c_id)
{
	sc_packet_login_fail packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_FAIL;
	clients[c_id].do_send(sizeof(packet), &packet);
}

//이동
void send_move_packet(int _id, int target)
{
	sc_packet_move packet;
	packet.id = target;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_MOVE;
	packet.x = clients[target].x;
	packet.y = clients[target].y;
	packet.move_time = clients[target].last_move_time;
	clients[_id].do_send(sizeof(packet), &packet);
}

//오브젝트 제거
void send_remove_object(int c_id, int victim)
{
	sc_packet_remove_object packet;
	packet.id = victim;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_REMOVE_OBJECT;
	clients[c_id].do_send(sizeof(packet), &packet);
}

//오브젝트 생성
void send_put_object(int c_id, int target)
{
	sc_packet_put_object packet;
	packet.id = target;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_PUT_OBJECT;
	packet.x = clients[target].x;
	packet.y = clients[target].y;
	strcpy_s(packet.name, clients[target].name);
	packet.object_type = 0;
	clients[c_id].do_send(sizeof(packet), &packet);
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
void send_status_packet(int c_id)
{
	sc_packet_status_change packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_STATUS_CHANGE;
	packet.maxhp = clients[c_id]._max_hp;
	packet.hp = clients[c_id]._hp;
	packet.level = clients[c_id]._level;
	packet.exp = clients[c_id]._exp;
	clients[c_id].do_send(sizeof(packet), &packet);
}

//해제
void Disconnect(int c_id)
{
	CLIENT& cl = clients[c_id];
	cl.vl.lock();
	unordered_set <int> my_vl = cl.viewlist;
	cl.vl.unlock();
	strcpy_s(clients[c_id].name, " ");
	for (auto& other : my_vl) {
		CLIENT& target = clients[other];
		
		if (ST_INGAME != target._state)
			continue;
		target.vl.lock();
		if (0 != target.viewlist.count(c_id)) {
			target.viewlist.erase(c_id);
			target.vl.unlock();
			send_remove_object(other, c_id);
		}
		else target.vl.unlock();
	}
	clients[c_id].state_lock.lock();
	clients[c_id]._state = ST_FREE;
	clients[c_id].state_lock.unlock();
	closesocket(clients[c_id]._socket);
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
void Timer_Event(int npc_id, int user_id, EVENT_TYPE ev, std::chrono::milliseconds ms)
{
	timer_ev order;
	order.this_id = npc_id;
	order.target_id = user_id;
	order.order = ev;
	order.start_t = chrono::system_clock::now() + ms;
	timer_q.push(order);
}

//패킷 판별
void process_packet(int client_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	CLIENT& cl = clients[client_id];

	switch (packet_type) {
	case CS_PACKET_LOGIN: {
		printf("login\n");
		break;
	}
	case CS_PACKET_MOVE: {
		printf("move\n");
		break;
	}
	case CS_PACKET_ATTACK: {
		printf("attack\n");
		break;
	}
	case CS_PACKET_CHAT:
		printf("chat\n");
		break;
	case CS_PACKET_TELEPORT:
		printf("teleport\n");
		break;
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

		int client_id = static_cast<int>(iocp_key);
		Overlap* exp_over = reinterpret_cast<Overlap*>(p_over);
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			cout << "GQCS Error : ";
			error_display(err_no);
			cout << endl;
			Disconnect(client_id);
			if (exp_over->_op == OP_SEND)
				delete exp_over;
			continue;
		}

		switch (exp_over->_op) {
		case OP_RECV: {
			if (num_byte == 0) {
				cout << "연결종료" << endl;
				
				Disconnect(client_id);
				continue;
			}
			CLIENT& cl = clients[client_id];
			int remain_data = num_byte + cl._prev_size;
			unsigned char* packet_start = exp_over->_net_buf;
			int packet_size = packet_start[0];

			while (packet_size <= remain_data) {
				process_packet(client_id, packet_start);
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
				Disconnect(client_id);
			}
			delete exp_over;
			break;
		}
		case OP_ACCEPT: {
			cout << "Accept Completed.\n";
			SOCKET c_socket = *(reinterpret_cast<SOCKET*>(exp_over->_net_buf));
			int n_id = get_id();
			if (-1 == n_id) {
				cout << "user over.\n";
			}
			else {
				cout << "user n_id "<<n_id << "accept.\n";
				CLIENT& cl = clients[n_id];
				cl.x = rand() % WORLD_WIDTH;
				cl.y = rand() % WORLD_HEIGHT;

				//cl.x = 200;
				//cl.y = 200;

				cl._id = n_id;
				cl._prev_size = 0;
				cl._recv_over._op = OP_RECV;
				cl._recv_over._wsa_buf.buf = reinterpret_cast<char*>(cl._recv_over._net_buf);
				cl._recv_over._wsa_buf.len = sizeof(cl._recv_over._net_buf);
				ZeroMemory(&cl._recv_over._wsa_over, sizeof(cl._recv_over._wsa_over));
				cl._socket = c_socket;

				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_h_iocp, n_id, 0);
				cl.do_recv();
			}

			ZeroMemory(&exp_over->_wsa_over, sizeof(exp_over->_wsa_over));
			c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
			*(reinterpret_cast<SOCKET*>(exp_over->_net_buf)) = c_socket;
			AcceptEx(sever_socket, c_socket, exp_over->_net_buf + 8, 0, sizeof(SOCKADDR_IN) + 16,
				sizeof(SOCKADDR_IN) + 16, NULL, &exp_over->_wsa_over);
		}
					  break;
		
		case OP_PLAYER_MOVE: {
			
			delete exp_over;
			break;
		}
		case OP_PLAYER_ATTACK: {
			
			delete exp_over;
			break;
		}
		case OP_PLAYER_RE: {
			clients[client_id]._hp = clients[client_id]._max_hp;
			clients[client_id]._exp = clients[client_id]._exp / 2;
			send_status_packet(client_id);
			delete exp_over;
			break;
		}
		}
	}
}


//타이머
void ev_timer() {

	while (true) {
		timer_ev order;
		timer_q.try_pop(order);
		//auto t = order.start_t - chrono::system_clock::now();
		int npc_id = order.this_id;
		
		if (clients[npc_id]._state != ST_INGAME) continue;
		if (clients[npc_id]._is_active == false) continue;
		if (order.start_t <= chrono::system_clock::now()) {
		

		}
		else {
			timer_q.push(order);
			this_thread::sleep_for(10ms);

		}


	}
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
		clients[i]._id = i;

	vector <thread> worker_threads;

	thread timer_thread{ ev_timer };
	for (int i = 0; i < 10; ++i)
		worker_threads.emplace_back(worker_thread);
	
	for (auto& th : worker_threads)
		th.join();

	timer_thread.join();
	for (auto& cl : clients) {
		if (ST_INGAME == cl._state)
			Disconnect(cl._id);
	}
	closesocket(sever_socket);
	WSACleanup();
}



//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------

