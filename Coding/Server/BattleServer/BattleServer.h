#pragma once
#include "pch.h"
#include "CorePch.h"
#include "IOCPServer.h"
#include "P_Manager.h"

class PManager;

class BattleServer :
	public IOCPServer
{
public:
	BattleServer();
	virtual ~BattleServer() {};
	void Run();
	void End();
	bool Connect();
	virtual bool OnRecv(int s_id, Overlap* exp_over, DWORD num_byte) override;
	virtual bool OnAccept(Overlap* exp_over) override;
	virtual bool OnEvent(int s_id, Overlap* exp_over) override;
	virtual void ReStart() override;

	void PutEvent(int target, int player_id, COMMAND type);
	//void TimerEvent(int np_s_id, int user_id, int ev, std::chrono::milliseconds ms);
	void ev_timer();
	void init_server();
	virtual bool Lobby_recv(Overlap* exp_over, DWORD num_byte) override; //Lobby 서버 받는 용도
	void Lobby_send(int num_bytes, void* mess);//Lobby 서버 받는 용도
	void Lobby_do_send(int num_bytes, void* mess);//Lobby 서버 받는 용도
	void Lobby_do_recv(); //Lobby 서버 받는 용도
	void Lobby_Login_SEND(); //Lobby 서버 받는 용도

	int _sid;
	short Port_num;
	int      _prev_size;
	Overlap _recv_over;
protected:
	SOCKET Lobby_socket; //로비와 연결하는 용도의 소켓
	std::unique_ptr<PManager> PM;
	thread timer_thread;

};

