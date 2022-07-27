#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif

#include "CorePch.h"
#include "Enum.h"

#include <thread>
#include <cmath>
#include<random>
#include <sqlext.h>  

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

mutex  g_snow_mutex;
mutex  g_ice_mutex;
mutex  g_item_mutex;
mutex  g_spitem_mutex;


const int RANGE = 10000;
const int BONFIRE_RANGE = 1700;

const int Tornado_id = 100;

struct timer_ev {

    int this_id = {};
    int target_id = {};
    chrono::system_clock::time_point   start_t = {};
    int order = {};
    constexpr bool operator < (const timer_ev& _Left) const
    {
        return (start_t > _Left.start_t);
    }
};

void error_display(int err_no);

#include "Overlap.h"
#include "CLIENT.h"
#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

struct global_arr {
    bool g_snow_drift[MAX_SNOWDRIFT] = {};
    bool g_ice_drift[MAX_SNOWDRIFT] = {};
    bool g_spitem[MAX_ITEM] = {};
    bool g_item[MAX_ITEM] = {};
    bool g_start_game = false;
    int g_tonardo_id = -1;
    bool g_tonardo = false;
};

extern global_arr GA;
extern array <CLIENT, MAX_USER> clients;

bool is_player(int id);
bool is_bonfire(int a);
bool is_near(int a, int b);
bool is_snowdrift(int obj_id);
bool is_icedrift(int obj_id);
bool is_item(int obj_id);
bool is_spitem(int obj_id);
void send_login_ok_packet(int _s_id);
void send_login_fail_packet(int _s_id, int _reason);
void send_remove_object(int _s_id, int victim);
void send_put_object(int _s_id, int target);
void send_chat_packet(int user_id, int my_id, char* mess);
void send_status_packet(int _s_id);
void send_move_packet(int _id, int target);
void send_hp_packet(int _id);
void send_is_bone_packet(int _id);
void send_game_end(int s_id, int target);
void send_state_change(int s_id, int target, int stat);
void send_player_count(int s_id, int bear, int snowman);

