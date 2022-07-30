#include "pch.h"
#include "CLIENT.h"

CLIENT::CLIENT() : cl_state(ST_FREE), _combat(COMBAT_END), _prev_size(0)
{
	_s_id = {}; //플레이어 배열 넘버
	ZeroMemory(&name, sizeof(name));
	ZeroMemory(&_id, sizeof(_id));
	ZeroMemory(_pw, sizeof(_pw));
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
	
	_hp = _max_hp; // 체력
	_at_range = 1; // 기본 공격 범위
	_sk_range = 2; // 스킬 범위
	is_bone = true;
	is_match = false;
	iMaxSnowballCount = iOriginMaxSnowballCount;
	iMaxIceballCount = iOriginMaxIceballCount;
	iMaxMatchCount = iOriginMaxMatchCount;
	iCurrentSnowballCount = 0;
	iCurrentIceballCount = 0;
	iCurrentMatchCount = 0;
	bHasUmbrella = false;
	bIsRiding = false;
	bHasBag = false;
	bHasShotGun = false;
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
	_s_id = {}; //플레이어 배열 넘버
	ZeroMemory(&name, sizeof(name));
	ZeroMemory(&_id, sizeof(_id));
	ZeroMemory(_pw, sizeof(_pw));
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

	_hp = _max_hp; // 체력
	_at_range = 1; // 기본 공격 범위
	_sk_range = 2; // 스킬 범위
	is_bone = true;
	is_match = false;
	iMaxSnowballCount = iOriginMaxSnowballCount;
	iMaxIceballCount = iOriginMaxIceballCount;
	iMaxMatchCount = iOriginMaxMatchCount;
	iCurrentSnowballCount = 0;
	iCurrentIceballCount = 0;
	iCurrentMatchCount = 0;
	bHasUmbrella = false;
	bIsRiding = false;
	bHasBag = false;
	bHasShotGun = false;
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
