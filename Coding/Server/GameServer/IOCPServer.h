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
	void Worker();
	void Disconnect(int _s_id);
	void Wsa_err_display(int err_no);
	void JoinThread();
protected:
	Overlap   accept_ex;
	HANDLE g_h_iocp;
	SOCKET sever_socket;
	int Workerth_num;
	vector <thread> worker_threads;
	

};

