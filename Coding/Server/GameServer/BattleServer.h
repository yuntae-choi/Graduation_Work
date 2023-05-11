#pragma once
#include "pch.h"
#include "CorePch.h"


class BattleServer 
{
public:
	BattleServer() : Match_Users(0), Port_Num(-1), state(SERVER_FREE)
	{
		
	};
	virtual ~BattleServer() {};

	void reset()
	{
		Match_Users = 0;
		Port_Num = -1;
	};
	bool Set_State(SERVER_STATE st);
	void Get_State(SERVER_STATE& st);
public:
	int Match_Users;
	int Port_Num;
protected:
	SERVER_STATE state;
	std::mutex state_lock;
};

