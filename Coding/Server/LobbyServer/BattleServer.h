#pragma once
#include "pch.h"
#include "CorePch.h"
#include <shellapi.h>

class BattleServer
{
public:
	BattleServer() : Match_Users(0), Port_Num(-1), state(SERVER_FREE), s_id(-1)
	{

	};
	virtual ~BattleServer() {};

	void run(int newid);

	void reset();
	bool Set_State(SERVER_STATE st);
	void Get_State(SERVER_STATE& st);
	bool Matching();
	bool create_server();
	void Lobby_do_send(int num_bytes, void* mess);//Lobby 서버 받는 용도
	void Lobby_do_recv(); //Lobby 서버 받는 용도
public:
	int Match_Users;
	short Port_Num;
	int s_id;
	int      _prev_size;
	Overlap _recv_over;
	SOCKET* Lobby_socket; //배틀서버와 연결하는 용도의 소켓
protected:
	SERVER_STATE state;
	std::mutex state_lock;
	
};

