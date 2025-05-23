#pragma once
#include "pch.h"
#include "CorePch.h"
#include<concurrent_priority_queue.h>

class LobbyServer;
class BattleServer;

class PManager
{
public:
	PManager() {};
	virtual ~PManager() {};
	void ProcessPacket(int Id, unsigned char* recv);
	void ProcessServerPacket(int Id, unsigned char* recv);
	bool Login();
	bool CreateAccount();
	void Move(int type);
	void Attack(int type);
	bool Damage();
	void ColdDamage(int Id);
	void Heal(int id, int type);
	void UseItem(int type);
	void GetItem();
	bool ItemBox(int type);
	void DelvelopCheat();
	void Snow(int type);
	bool Freeze();
	bool MATCHING(int id);
	bool GunFire();
	void Ready();
	bool SetStaus();
	void SendPlayerInfo(int _s_id);
	void PutSupplyBox();
	void SetBulletPos(int* r_arr);
	void SetPos(int s_id, char* _id, char* _pw, float _z);
	int get_id();
	void TimerEvent(int np_s_id, int user_id, int ev, std::chrono::milliseconds ms);
	int set_port();
	bool ServerLogin();
	bool Connect_Socket();
	bool Send_Server_Login_Ok();
	bool Server_Restart();
	HANDLE g_timer;
	static concurrency::concurrent_priority_queue <timer_ev> timer_q;;
	//LockQueue<timer_ev> timer_q;
private:
	CLIENT* cl = nullptr;
	BattleServer* b_server = nullptr;
	int IdNum = 0;
	unsigned char* RecvBuf = nullptr;
};

