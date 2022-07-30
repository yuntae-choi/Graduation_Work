#include "pch.h"

global_arr GA;
array <CLIENT, MAX_USER + MAX_NPC> clients;

void error_display(int err_no)
{
    WCHAR* lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err_no,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, 0);
    wcout << lpMsgBuf << endl;
    //while (true);
    LocalFree(lpMsgBuf);
}

//플레이어 판별
bool is_player(int id)
{
	return (id >= 0) && (id < MAX_USER);
}

//모닥불 범위 판정
bool is_bonfire(int a)
{
	if (BONFIRE_RANGE < abs(clients[a].x)) return false;
	if (BONFIRE_RANGE < abs(clients[a].y)) return false;
	return true;
}

//근처 객체 판별
bool is_near(int a, int b)
{
	if (RANGE < abs(clients[a].x - clients[b].x)) return false;
	if (RANGE < abs(clients[a].y - clients[b].y)) return false;
	return true;
}

bool is_snowdrift(int obj_id)
{
	unique_lock<mutex> _lock(g_snow_mutex);
	if (GA.g_snow_drift[obj_id]) {
		GA.g_snow_drift[obj_id] = false;
		return true;
	}
	return false;
}

bool is_icedrift(int obj_id)
{
	unique_lock<mutex> _lock(g_snow_mutex);
	if (GA.g_ice_drift[obj_id]) {
		GA.g_ice_drift[obj_id] = false;
		return true;
	}
	return false;
}

bool is_item(int obj_id)
{
	unique_lock<mutex> _lock(g_item_mutex);
	if (GA.g_item[obj_id]) {
		GA.g_item[obj_id] = false;
		return true;
	}
	return false;
}

bool is_spitem(int obj_id)
{
	unique_lock<mutex> _lock(g_spitem_mutex);
	if (GA.g_spitem[obj_id]) {
		GA.g_spitem[obj_id] = false;
		return true;
	}
	return false;
}
//로그인 허용
void send_login_ok_packet(int _s_id)
{
	sc_packet_login_ok packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_OK;
	packet.s_id = _s_id;
	packet.color = clients[_s_id].color;
	packet.x = clients[_s_id].x;
	packet.y = clients[_s_id].y;
	packet.z = clients[_s_id].z;
	packet.yaw = clients[_s_id].Yaw;
	strcpy_s(packet.id, clients[_s_id]._id);
	clients[_s_id].do_send(sizeof(packet), &packet);
}


//로그인 실패
void send_login_fail_packet(int _s_id, int _reason)
{
	sc_packet_login_fail packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_LOGIN_FAIL;
	packet.reason = _reason;
	clients[_s_id].do_send(sizeof(packet), &packet);
}

//오브젝트 제거
void send_remove_object(int _s_id, int victim)
{
	sc_packet_remove_object packet;
	packet.s_id = victim;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_REMOVE_OBJECT;
	clients[_s_id].do_send(sizeof(packet), &packet);
}

//오브젝트 생성
void send_put_object(int _s_id, int target)
{
	sc_packet_put_object packet;
	packet.s_id = target;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_PUT_OBJECT;
	packet.x = clients[target].x;
	packet.y = clients[target].y;
	packet.z = clients[target].z;

	strcpy_s(packet.name, clients[target].name);
	packet.object_type = 0;
	clients[_s_id].do_send(sizeof(packet), &packet);
}

//메세지 전송
void send_chat_packet(int user_id, int my_id, char* mess)
{
	sc_packet_chat packet;
	packet.id = my_id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_CHAT;
	strcpy_s(packet.message, mess);
	clients[user_id].do_send(sizeof(packet), &packet);
}

//플레이어 데이터 변경
void send_status_packet(int _s_id)
{
	sc_packet_status_change packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_STATUS_CHANGE;
	clients[_s_id].do_send(sizeof(packet), &packet);
}

//이동
void send_move_packet(int _id, int target)
{
	cs_packet_move packet;
	packet.sessionID = target;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_MOVE;
	packet.x = clients[target].x;
	packet.y = clients[target].y;
	packet.z = clients[target].z;
	packet.yaw = clients[target].Yaw;
	packet.vx = clients[target].VX;
	packet.vy = clients[target].VY;
	packet.vz = clients[target].VZ;
	packet.direction = clients[target].direction;
	clients[_id].do_send(sizeof(packet), &packet);
}

//이동
void send_hp_packet(int _id)
{
	sc_packet_hp_change packet;
	packet.s_id = _id;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_HP;
	packet.hp = clients[_id]._hp;

	// printf_s("[Send hp change] id : %d, hp : %d\n", packet.s_id, packet.hp);
	clients[_id].do_send(sizeof(packet), &packet);
}

//모닥불 확인
void send_is_bone_packet(int _id)
{
	cout << "모닥불 확인" << endl;
	sc_packet_is_bone packet;
	packet.size = sizeof(packet);
	packet.type = SC_PACKET_IS_BONE;
	// printf_s("[Send hp change] id : %d, hp : %d\n", packet.s_id, packet.hp);
	clients[_id].do_send(sizeof(packet), &packet);
}


void send_game_end(int s_id, int target)
{
	sc_packet_game_end _packet;
	_packet.size = sizeof(_packet);
	_packet.type = SC_PACKET_END;
	_packet.s_id = s_id;
	clients[target].do_send(sizeof(_packet), &_packet);
}

void send_state_change(int s_id, int target, int stat)
{
	sc_packet_status_change _packet;
	_packet.size = sizeof(_packet);
	_packet.type = SC_PACKET_STATUS_CHANGE;
	_packet.state = stat;
	_packet.s_id = s_id;
	clients[target].do_send(sizeof(_packet), &_packet);
}

void send_player_count(int s_id, int bear, int snowman)
{
	sc_packet_player_count _packet;
	_packet.size = sizeof(_packet);
	_packet.type = SC_PACKET_PLAYER_COUNT;
	_packet.bear = bear;
	_packet.snowman = snowman;
	clients[s_id].do_send(sizeof(_packet), &_packet);
}

void send_kill_log(int s_id, int attacker, int victim, int cause)
{
	sc_packet_kill_log _packet;
	_packet.size = sizeof(_packet);
	_packet.type = SC_PACKET_KILL_LOG;
	_packet.attacker = attacker;
	_packet.victim = victim;
	_packet.cause = cause;
	clients[s_id].do_send(sizeof(_packet), &_packet);
};
