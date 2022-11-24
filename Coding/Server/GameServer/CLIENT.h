#pragma once
#include "pch.h"
#include "CorePch.h"
#include "Enum.h"

class CLIENT
{
public:
	int _s_id; //플레이어 배열 넘버
	char name[MAX_NAME_SIZE]; //플레이어 nick
	char _id[MAX_NAME_SIZE]; // id
	char _pw[MAX_NAME_SIZE];  // pw
		// 위치
	float	x;
	float	y;
	float	z;
	// 회전값
	float	Yaw;
	float	Pitch;
	float	Roll;
	// 속도
	float VX;
	float VY;
	float VZ;

	float direction;

	int _hp; // 체력
	int _at_range; // 기본 공격 범위
	int _sk_range; // 스킬 범위
	bool is_bone;
	atomic_bool is_match;

	int32 iMaxSnowballCount;
	int32 iMaxIceballCount;
	int32 iMaxMatchCount;
	const int32 _max_hp = 390;	// 성냥 최대보유량 (초기)
	const int32 _min_hp = 270;// 눈 최대보유량 (초기)
	const int32 _BeginSlowHP = 300;	// 캐릭터가 슬로우 상태가 되기 시작하는 hp

	const int32 iOriginMaxMatchCount = 2;	// 성냥 최대보유량 (초기)
	const int32 iOriginMaxSnowballCount = 10;// 눈 최대보유량 (초기)
	const int32 iOriginMaxIceballCount = 10;     // 얼음 최대보유량 (초기)

	const int32 iBagMaxMatchCount = 3;     // 성냥 최대보유량 (가방)
	const int32 iBagMaxSnowballCount = 15;// 눈 최대보유량 (가방)
	const int32 iBagMaxIceballCount = 15;     // 얼음 최대보유량 (가방)

	int32 iCurrentSnowballCount;
	int32 iCurrentIceballCount;
	int32 iCurrentMatchCount;
	bool bHasUmbrella;
	bool bIsRiding;
	bool bHasBag;
	bool bHasShotGun;
	bool bIsSnowman;	// 현재 캐릭터가 눈사람인지
	bool b_ready;
	atomic_bool dot_dam ;


	unordered_set   <int>  viewlist; // 시야 안 오브젝트
	mutex vl;
	mutex hp_lock;
	mutex lua_lock;
	mutex state_lock;
	mutex cl_state_lock;
	mutex pl_state_lock;


	CL_STATE cl_state;  //  접속 상태
	STATE  pl_state;    //인게임 상태
	atomic_bool   _is_active;

	ATTACK _combat;

	atomic_int    _count;

	Overlap _recv_over;
	SOCKET  _socket;
	int      _prev_size;
	int      last_move_time;
	int      color;
public:
	CLIENT(); 
	~CLIENT()
	{
		closesocket(_socket);
	}
	void data_init();
	void do_recv();
	void do_send(int num_bytes, void* mess);
	
	bool is_cl_state(CL_STATE value)
	{
		lock_guard<mutex> lock(cl_state_lock);		
		return (value == cl_state);
	}
	bool is_pl_state(STATE value)
	{
		lock_guard<mutex> lock(pl_state_lock);
		return (value == pl_state);
	}

	void set_cl_state(CL_STATE value)
	{
		lock_guard<mutex> lock(cl_state_lock);
	    cl_state = value;
	}

	void set_pl_state(STATE value)
	{
		lock_guard<mutex> lock(pl_state_lock);
		pl_state = value;
	}
};

