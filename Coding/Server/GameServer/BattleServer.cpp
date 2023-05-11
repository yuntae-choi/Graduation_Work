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