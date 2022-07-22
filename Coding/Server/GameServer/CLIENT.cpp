#include "pch.h"
#include "CLIENT.h"

CLIENT::CLIENT() : _state(ST_FREE), _combat(COMBAT_END), _prev_size(0)
{
	_s_id = {}; //플레이어 배열 넘버
	// 위치
	x = {};
	y = {};
	z = {};
	// 회전값
	Yaw = {};
	Pitch = {};
	Roll = {};
	// 속도
	VX = {};
	VY = {};
	VZ = {};
	direction = {};
	_max_hp = 390; // 최대 체력
	_min_hp = 270;
	_BeginSlowHP = 300;	// 캐릭터가 슬로우 상태가 되기 시작하는 hp

	_hp = {}; // 체력
	_at_range = 1; // 기본 공격 범위
	_sk_range = 2; // 스킬 범위
	is_bone = true;
	is_match = false;
	iMaxSnowballCount = 10;
	iMaxMatchCount = 3;
	iOriginMaxMatchCount = 2;	// 성냥 최대보유량 (초기, 가방x)
	iOriginMaxSnowballCount = 10;
	iCurrentSnowballCount = 0;
	iCurrentMatchCount = 0;
	bHasUmbrella = false;
	bIsRiding = false;
	bHasBag = false;
	bIsSnowman = false;	// 현재 캐릭터가 눈사람인지
	b_ready = false;
	dot_dam = false;
	viewlist = {}; // 시야 안 오브젝트
	_is_active = false;
	_count = {};
	_recv_over = {};
	_prev_size = {};
	last_move_time = {};
}

void CLIENT::do_recv()
{
	DWORD recv_flag = 0;
	ZeroMemory(&_recv_over._wsa_over, sizeof(_recv_over._wsa_over));
	_recv_over._wsa_buf.buf = reinterpret_cast<char*>(_recv_over._net_buf + _prev_size);
	_recv_over._wsa_buf.len = sizeof(_recv_over._net_buf) - _prev_size;
	int ret = WSARecv(_socket, &_recv_over._wsa_buf, 1, 0, &recv_flag, &_recv_over._wsa_over, NULL);
	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
		if (ERROR_IO_PENDING != error_num)
			error_display(error_num);
	}
}

void CLIENT::do_send(int num_bytes, void* mess)
{
	Overlap* ex_over = new Overlap(COMMAND::OP_SEND, num_bytes, mess);
	int ret = WSASend(_socket, &ex_over->_wsa_buf, 1, 0, 0, &ex_over->_wsa_over, NULL);
	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
		if (ERROR_IO_PENDING != error_num)
			error_display(error_num);
	}
}

void CLIENT::data_init()
{
	int _max_hp = 390; // 최대 체력
	int _min_hp = 270;
	int _BeginSlowHP = 300;	// 캐릭터가 슬로우 상태가 되기 시작하는 hp
	int _hp = _max_hp; // 체력 범위
	bool is_bone = true;
	atomic_bool is_match = false;
	atomic_bool   _is_active = false;
	int32 iMaxSnowballCount = 10;
	int32 iOriginMaxSnowballCount = 10;	// 눈덩이 최대보유량 (초기, 가방x)
	int32 iOriginMaxMatchCount = 2;	// 성냥 최대보유량 (초기, 가방x)
	int32 iMaxMatchCount = 3;
	int32 iCurrentSnowballCount = 0;
	int32 iCurrentMatchCount = 0;
	bool bHasUmbrella = false;
	bool bHasBag = false;
	bool bIsSnowman = false;	// 현재 캐릭터가 눈사람인지
	bool b_ready = false;
}
