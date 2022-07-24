// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// winsock2 사용을 위해 아래 코멘트 추가
#pragma comment(lib, "ws2_32.lib")
#include "HoloLens/AllowWindowsPlatformAtomics.h"
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <chrono>
#include <mutex>
#include "HoloLens/HideWindowsPlatformAtomics.h"

#include "Final_Project.h"

#define	MAX_BUFFER		4096
#define SERVER_PORT		9090
//#define SERVER_IP		"112.148.142.95" // 외부 IP
//#define SERVER_IP		"192.168.219.106" //로컬IP
#define SERVER_IP		"127.0.0.1" //로컬IP
#define MAX_CLIENTS		100

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

const int32  MAX_NAME_SIZE = 20;
const int32  MAX_CHAT_SIZE = 100;
const int32 BUF_SIZE = 2048;
const int32 Tornado_id = 100;
const int32  MAX_BULLET_RANG = 8;

// 소켓 통신 구조체

// 패킷 정보
const char CS_PACKET_LOGIN = 1;
const char CS_PACKET_MOVE = 2;
const char CS_PACKET_ATTACK = 3;
const char CS_PACKET_CHAT = 4;
const char CS_PACKET_TELEPORT = 5;
const char CS_PACKET_THROW_SNOW = 6;
const char CS_PACKET_DAMAGE = 7;
const char CS_PACKET_GET_ITEM = 8;
const char CS_PACKET_LOGOUT = 9;
const char CS_PACKET_STATUS_CHANGE = 10;
const char CS_PACKET_READY = 11;
const char CS_PACKET_STOP_SNOW_FARMING = 12;
const char CS_PACKET_MATCH = 13;
const char CS_PACKET_OPEN_BOX = 14;
const char CS_PACKET_GUNATTACK = 15;
const char CS_PACKET_GUNFIRE = 16;
const char CS_PACKET_UMB = 17;
const char CS_PACKET_ACCOUNT = 18;

const char SC_PACKET_LOGIN_OK = 1;
const char SC_PACKET_MOVE = 2;
const char SC_PACKET_PUT_OBJECT = 3;
const char SC_PACKET_REMOVE_OBJECT = 4;
const char SC_PACKET_CHAT = 5;
const char SC_PACKET_LOGIN_FAIL = 6;
const char SC_PACKET_STATUS_CHANGE = 7;
const char SC_PACKET_DISCONNECT = 8;
const char SC_PACKET_HP = 9;
const char SC_PACKET_THROW_SNOW = 10;
const char SC_PACKET_ATTACK = 11;
const char SC_PACKET_GET_ITEM = 12;
const char SC_PACKET_READY = 13;
const char SC_PACKET_START = 14;
const char SC_PACKET_STOP_SNOW_FARMING = 15;
const char SC_PACKET_IS_BONE = 16;
const char SC_PACKET_LOGOUT = 17;
const char SC_PACKET_END = 18;
const char SC_PACKET_OPEN_BOX = 19;
const char SC_PACKET_GUNATTACK = 20;
const char SC_PACKET_GUNFIRE = 21;
const char SC_PACKET_TELEPORT = 22;
const char SC_PACKET_UMB = 23;
const char SC_PACKET_ACCOUNT = 24;



enum COMMAND_Type
{
	COMMAND_SNOWBALL = 1,
	COMMAND_ICEBALL,
	COMMAND_MOVE,
	COMMAND_DAMAGE,
	COMMAND_MATCH,
	COMMAND_THROW_SB,
	COMMAND_THROW_IB,
	COMMAND_CANCEL_SB,
	COMMAND_CANCEL_IB,
	COMMAND_SHOTGUN,
	COMMAND_GUNFIRE,
	COMMAND_UMB_START,
	COMMAND_UMB_END
};

enum TELEPORT_Type
{
	TEL_FIRE = 1,
	TEL_BRIDGE,
	TEL_TOWER,
	TEL_ICE
};

enum CHEAT_Type
{
	CHEAT_HP_UP = 1,
	CHEAT_HP_DOWN,
	CHEAT_SNOW_PLUS,
	CHEAT_ICE_PLUS
};

enum ITEM_Type
{
	ITEM_MAT = 0,
	ITEM_UMB,
	ITEM_BAG,
	ITEM_SNOW,
	ITEM_JET,
	ITEM_ICE
};

enum ATTACK_Type
{
	ATTACK_SNOWBALL = 0,
	ATTACK_ICEBALL,
	ATTACK_SHOTGUN,
	END_SNOWBALL,
	END_ICEBALL,
	END_SHOTGUN,
	CANCEL_SNOWBALL,
	CANCEL_ICEBALL
};

enum BULLET_Type
{
	BULLET_SNOWBALL = 0,
	BULLET_ICEBALL,
	BULLET_SNOWBOMB
};

enum OBJ_Type { PLAYER,ITEM_BOX, TONARDO };

enum Login_fail_Type { OVERLAP_ID, WORNG_ID, WORNG_PW, OVERLAP_AC, CREATE_AC};


// 패킷

#pragma pack (push, 1)
struct cs_packet_login {
	unsigned char size;
	char	type;
	char	id[MAX_NAME_SIZE];
	char	pw[MAX_NAME_SIZE];
	float z;
};

struct sc_packet_login_ok {
	unsigned char size;
	char type;
	// 세션 아이디
	int32		s_id;
	float x, y, z;
	float yaw;
	char	id[MAX_NAME_SIZE];
	char	pw[MAX_NAME_SIZE];
};

struct cs_packet_logout {
	unsigned char size;
	char	type;
	int32     s_id;
};

struct cs_packet_move {
	unsigned char size;
	char	type;
	//char	direction;			// 0 : up,  1: down, 2:left, 3:right
	int32 sessionID;
	float x, y, z;
	// 속도
	float vx, vy, vz;
	// 회전값
	float yaw;

	float direction;
	//long long move_time;
};

struct sc_packet_put_object {
	unsigned char size;
	char type;
	int32 s_id;
	float x, y, z;
	float yaw;
	char object_type;
	char	name[MAX_NAME_SIZE];
};

struct cs_packet_throw_snow {
	unsigned char size;
	char	type;
	int32 s_id;
	int32 bullet;
	bool    mode;
	float x, y, z;
	float yaw, pitch, roll;
	float speed;
};

struct cs_packet_fire {
	unsigned char size;
	char	type;
	int32 s_id;
	float pitch;
	int32 rand_int[MAX_BULLET_RANG];
};

struct cs_packet_damage {
	unsigned char size;
	char	type;
};

struct cs_packet_match {
	unsigned char size;
	char	type;
};

struct cs_packet_umb {
	unsigned char size;
	char	type;
	int     s_id;
	bool    end;
};

struct sc_packet_hp_change {
	unsigned char size;
	char type;
	int32 s_id;
	int32 hp;
};

struct cs_packet_attack {
	unsigned char size;
	char	type;
	int32 s_id;
	int32 bullet;

};

struct cs_packet_shotattack {
	unsigned char size;
	char	type;
	int32 s_id;
};



struct cs_packet_chat {
	unsigned char size;
	char	type;
	int32 s_id;
	int32 cheat_type;
};

struct cs_packet_teleport {
	unsigned char size;
	char	type;
	int	    Point;

};

struct cs_packet_get_item {
	unsigned char size;
	char	type;
	int32 s_id;
	int32 item_type;
	int32 current_bullet;
	int32 destroy_obj_id;
};

struct cs_packet_open_box {
	unsigned char size;
	char	type;
	int32 open_obj_id;
};


struct sc_packet_move {
	unsigned char size;
	char type;
	int32		id;
	short  x, y;
	char move_time[MAX_CHAT_SIZE];
};

struct sc_packet_remove_object {
	unsigned char size;
	char type;
	int32 s_id;
};

struct sc_packet_chat {
	unsigned char size;
	char type;
	int32 id;
	char message[MAX_CHAT_SIZE];
};

struct sc_packet_login_fail {
	unsigned char size;
	char type;
	int32	 reason;		
};

struct sc_packet_status_change {
	unsigned char size;
	char type;
	int32 s_id;
	short   state;
};

struct sc_packet_ready { // 타 플레이어 레디
	unsigned char size;
	char	type;
	int32	s_id;
};

struct cs_packet_ready { // 게임 레디 요청
	unsigned char size;
	char	type;
};

struct sc_packet_start { // 스폰
	unsigned char size;
	char type;
};
struct sc_packet_is_bone {
	unsigned char size;
	char	type;
	
};

struct sc_packet_game_end {
	unsigned char size;
	char	type;
	int     s_id;
};



enum OPTYPE { OP_SEND, OP_RECV, OP_DO_MOVE };

class Overlap {
public:
	WSAOVERLAPPED   _wsa_over;
	OPTYPE         _op;
	WSABUF         _wsa_buf;
	unsigned char   _net_buf[BUF_SIZE];
	int32            _target;
public:
	Overlap(OPTYPE _op, char num_bytes, void* mess) : _op(_op)
	{
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		_wsa_buf.buf = reinterpret_cast<char*>(_net_buf);
		_wsa_buf.len = num_bytes;
		memcpy(_net_buf, mess, num_bytes);
	}

	Overlap(OPTYPE _op) : _op(_op) {}

	Overlap()
	{
		_op = OP_RECV;
	}

	~Overlap()
	{
	}
};



class FINAL_PROJECT_API NetworkData
{
public:
};
