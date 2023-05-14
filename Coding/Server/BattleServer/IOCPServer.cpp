#include "pch.h"
#include "IOCPServer.h"


IOCPServer::IOCPServer() : state(SERVER_FREE)
{
}

IOCPServer::~IOCPServer()
{
	for (auto& th : worker_threads)
		th.join();
	for (auto& cl : clients) {
		if (ST_INGAME == cl.cl_state)
			Disconnect(cl._s_id);
	}

	WSACleanup();
}

bool IOCPServer::Init(const int worker_num)
{
	WSADATA WSAData;
	int ret = WSAStartup(MAKEWORD(2, 2), &WSAData);
	Wsa_err_display(ret);
	sever_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	Workerth_num = worker_num;

	for (int i = 0; i < MAX_SNOWDRIFT; ++i) GA.g_snow_drift[i] = true;
	for (int i = 0; i < MAX_SNOWDRIFT; ++i) GA.g_ice_drift[i] = true;
	for (int i = 0; i < MAX_ITEM; ++i) GA.g_item[i] = true;
	for (int i = 0; i < MAX_ITEM; ++i) GA.g_spitem[i] = true;
	for (int i = 0; i < MAX_USER; ++i) clients[i]._s_id = i;


	return true;
};

bool IOCPServer::BindListen(const int port_num)
{
	if (port_num == 0) return false;
	DWORD dwBytes;
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_num);
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
	AcceptEx(sever_socket, c_socket, accept_buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, &accept_ex._wsa_over);


	return true;
};

void IOCPServer::Start()
{
	CreateWorker();

};

void IOCPServer::ReStart()
{

};

void IOCPServer::CreateWorker()
{
	for (int i = 0; i < Workerth_num; ++i)
		worker_threads.emplace_back([this]() {Worker(); });
};

void IOCPServer::Worker()
{
	while (1) {
		DWORD num_byte;
		//DWORD dwBytes;
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
			if (false == OnRecv(_s_id, exp_over, num_byte))
				continue;
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
			OnAccept(exp_over);
			break;
		}
		case OP_PLAYER_HEAL: {

			OnEvent(_s_id, exp_over);
			delete exp_over;
			break;

		}
		case OP_PLAYER_DAMAGE: {
			OnEvent(_s_id, exp_over);
			delete exp_over;
			break;
		}
		case OP_OBJ_SPAWN: {
			OnEvent(_s_id, exp_over);
			delete exp_over;
			break;
		}
		case OP_SERVER_RECV: {
			if (false == Lobby_recv(exp_over, num_byte))
				continue;
			break;
		}
		case OP_SERVER_SEND: {
			if (num_byte != exp_over->_wsa_buf.len) {
				cout << "send 에러" << endl;
				Disconnect(_s_id);
			}
			delete exp_over;
			break;
		}
		case OP_SERVER_RECYCLE: {
			if (num_byte != exp_over->_wsa_buf.len) {
				cout << "send 에러" << endl;
				Disconnect(_s_id);
			}
			delete exp_over;
			break;
		}
		default:
			break;
		}


	}
};


//해제
void IOCPServer::Disconnect(int _s_id)
{
	CLIENT& cl = clients[_s_id];
	//cl.vl.lock();
	//unordered_set <int> my_vl = cl.viewlist;
	//cl.vl.unlock();

	strcpy_s(clients[_s_id]._id, " ");

	/*for (auto& other : my_vl) {
		CLIENT& target = clients[other];

		if (ST_INGAME != target.cl_state)
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
		if (ST_INGAME != other.cl_state) {
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
	clients[_s_id].cl_state = ST_FREE;

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

void IOCPServer::JoinThread()
{
	for (auto& th : worker_threads)
		th.join();

}

void IOCPServer::Wsa_err_display(int err_no)
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

bool IOCPServer::Set_State(SERVER_STATE st)
{
	state_lock.lock();
	if (state != st) {
		state = st;
		state_lock.unlock();
		return true;
	}
	state_lock.unlock();
	return false;

};
void IOCPServer::Get_State(SERVER_STATE& st)
{
	state_lock.lock();
	st = state;
	state_lock.unlock();
};