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

	int _max_hp; // 최대 체력
	int _min_hp;
	int _BeginSlowHP;	// 캐릭터가 슬로우 상태가 되기 시작하는 hp

	int _hp; // 체력
	int _at_range; // 기본 공격 범위
	int _sk_range; // 스킬 범위
	bool is_bone;
	atomic_bool is_match;

	int32 iMaxSnowballCount;
	int32 iMaxMatchCount;
	int32 iOriginMaxMatchCount;	// 성냥 최대보유량 (초기, 가방x)
	int32 iOriginMaxSnowballCount;
	int32 iCurrentSnowballCount;
	int32 iCurrentMatchCount;
	bool bHasUmbrella;
	bool bIsRiding;
	bool bHasBag;
	bool bIsSnowman;	// 현재 캐릭터가 눈사람인지
	bool b_ready;
	atomic_bool dot_dam ;


	unordered_set   <int>  viewlist; // 시야 안 오브젝트
	mutex vl;
	mutex hp_lock;
	mutex lua_lock;

	mutex state_lock;
	CL_STATE _state;
	atomic_bool   _is_active;

	COMBAT _combat;

	atomic_int    _count;

	Overlap _recv_over;
	SOCKET  _socket;
	int      _prev_size;
	int      last_move_time;
public:
	CLIENT(); 
	~CLIENT()
	{
		closesocket(_socket);
	}
	void data_init();
	void do_recv();
	void do_send(int num_bytes, void* mess);

};

