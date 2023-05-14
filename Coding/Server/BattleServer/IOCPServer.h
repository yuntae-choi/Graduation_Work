#pragma once
#include "pch.h"
#include "CorePch.h"

class IOCPServer
{
public:
	IOCPServer();
	virtual ~IOCPServer();
	void Start();
	void CreateWorker();//작업쓰레드 생성
	bool Init(const int worker_num);
	bool BindListen(const int port_num);
	virtual bool OnRecv(int s_id, Overlap* exp_over, DWORD num_bytes) { return true; };
	virtual bool OnAccept(Overlap* exp_over) { return false; };
	virtual bool OnEvent(int s_id, Overlap* exp_over) { return true; };
	virtual bool Lobby_recv(Overlap* exp_over, DWORD num_byte) { return true; }; //Lobby 서버 받는 용도
	void Worker();
	void Disconnect(int _s_id);
	void Wsa_err_display(int err_no);
	void JoinThread();
	bool Set_State(SERVER_STATE st);
	void Get_State(SERVER_STATE& st);
	virtual void ReStart();
protected:
	Overlap   accept_ex;
	HANDLE g_h_iocp;
	SOCKET sever_socket;
	int Workerth_num;
	vector <thread> worker_threads;
	SERVER_STATE state;
	std::mutex state_lock;
};

