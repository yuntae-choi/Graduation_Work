#include "pch.h"
#include "LobbyServer.h"
#include "P_Manager.h"
#include "BattleServer.h"
using namespace std;
bool BattleServer::Set_State(SERVER_STATE st)
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
void BattleServer::Get_State(SERVER_STATE& st)
{
	state_lock.lock();
	st = state;
	state_lock.unlock();
};

void BattleServer::Lobby_do_recv()
{
	DWORD recv_flag = 0;
	ZeroMemory(&_recv_over._wsa_over, sizeof(_recv_over._wsa_over));
	_recv_over._op = OP_SERVER_RECV;
	_recv_over._wsa_buf.buf = reinterpret_cast<char*>(_recv_over._net_buf + _prev_size);
	_recv_over._wsa_buf.len = sizeof(_recv_over._net_buf) - _prev_size;
	int ret = WSARecv(*Lobby_socket, &_recv_over._wsa_buf, 1, 0, &recv_flag, &_recv_over._wsa_over, NULL);
	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
		if (ERROR_IO_PENDING != error_num)
			error_display(error_num);
	}
}


void BattleServer::Lobby_do_send(int num_bytes, void* mess)
{
	Overlap* ex_over = new Overlap(COMMAND::OP_SERVER_SEND, num_bytes, mess);
	int ret = WSASend(*Lobby_socket, &ex_over->_wsa_buf, 1, 0, 0, &ex_over->_wsa_over, NULL);
	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
		if (ERROR_IO_PENDING != error_num)
			error_display(error_num);
	}
}

void BattleServer::run(int newid)
{
	Port_Num = SERVER_PORT + newid;	//포트넘버
	wchar_t tmp[20];
	_itow_s(Port_Num, tmp, 10);
	ShellExecute(NULL, TEXT("open"), TEXT("Release\\BattleServer.exe"), tmp, NULL, SW_SHOW);
}

void BattleServer::reset()
{
	Match_Users = 0;
	Port_Num = -1;
};

bool BattleServer::Matching()
{
	state_lock.lock();
	if (state == SERVER_MATHCING && Match_Users < MAX_MATCH_USER) {
		Match_Users++;
		if (Match_Users == MAX_MATCH_USER)
			state == SERVER_USING;
		state_lock.unlock();
		return true;
	}
	state_lock.unlock();
	return false;
};

bool BattleServer::create_server()
{
	state_lock.lock();
	if (state == SERVER_FREE && Match_Users < MAX_MATCH_USER) {
		Match_Users = 0;
		state == SERVER_MATHCING;
		state_lock.unlock();
		return true;
	}
	state_lock.unlock();
	return false;
};