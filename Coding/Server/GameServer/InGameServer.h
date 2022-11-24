#pragma once
#include "pch.h"
#include "CorePch.h"
#include "IOCPServer.h"
#include "P_Manager.h"
class PManager;
class InGameServer :
	public IOCPServer
{
public:
	InGameServer();
	virtual ~InGameServer() {};
	void Run();
	void End();
	virtual bool OnRecv(int s_id, Overlap* exp_over, DWORD num_byte) override;
	virtual bool OnAccept(Overlap* exp_over) override;
	virtual bool OnEvent(int s_id, Overlap* exp_over) override;
	void PutEvent(int target, int player_id, COMMAND type);
	//void TimerEvent(int np_s_id, int user_id, int ev, std::chrono::milliseconds ms);
	void ev_timer();
private:
	std::unique_ptr<PManager> PM;
	thread timer_thread;
	
};

