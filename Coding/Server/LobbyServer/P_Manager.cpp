#include "pch.h"
#include "P_Manager.h"
#include "GameDataBase.h"
#include "LobbyServer.h"
#include "LockQueue.h"

using namespace std;

default_random_engine dre;
uniform_int_distribution<> uid;// ���� ����
uniform_int_distribution<> sprang;// ���� �ڽ� ���� ����
concurrency::concurrent_priority_queue<timer_ev> PManager::timer_q = concurrency::concurrent_priority_queue<timer_ev>();


void PManager::ProcessPacket(int Id, unsigned char* recv)
{
	IdNum = Id;
	RecvBuf = recv;;
	cl = &clients[IdNum];
	unsigned char packet_type = recv[1];

	switch (packet_type) {
	case CS_PACKET_LOGIN: {
		Login();
		break;
	}
	case CS_PACKET_ACCOUNT: {
		CreateAccount();
		break;
	}
	case CS_PACKET_MOVE: {
		Move(CS_PACKET_MOVE);
		break;
	}
	case CS_PACKET_ATTACK: {
		Attack(HAND);
		break;
	}
	case CS_PACKET_GUNATTACK: {
		Attack(GUN);
		break;
	}
	case CS_PACKET_DAMAGE: {
		Damage();
		break;
	}
	case CS_PACKET_MATCH: {
		Heal(IdNum, MATCH);
		break;
	}
	case CS_PACKET_UMB: {
		UseItem(ITEM_UMB);
		break;
	}
	case CS_PACKET_CHAT: {
		DelvelopCheat();
		break;
	}
	case CS_PACKET_GET_ITEM: {
		GetItem();
		break;
	}
	case CS_PACKET_THROW_SNOW: {
		Snow(SNOW_CREATE);
		break;
	}
	case CS_PACKET_CANCEL_SNOW: {
		Snow(SNOW_DESTORY);
		break;
	}
	case CS_PACKET_GUNFIRE: {
		GunFire();
		break;
	}
	case CS_PACKET_LOGOUT: {
		cout << "[Recv logout]";
		//Disconnect(s_id);
		break;
	}
	case CS_PACKET_STATUS_CHANGE: {
		cout << "STATUS_CHANGE" << endl;;
		SetStaus();
		break;
	}
	case CS_PACKET_READY: {
		Ready();
		break;
	}
	case CS_PACKET_OPEN_BOX: {
		ItemBox(CS_PACKET_OPEN_BOX);
		break;
	}
	case CS_PACKET_PUT_OBJECT: {
		ItemBox(CS_PACKET_PUT_OBJECT);
		break;
	}
	case CS_PACKET_NPC_MOVE: {
		Move(CS_PACKET_NPC_MOVE);
		break;
	}
	case CS_PACKET_FREEZE: {
		Freeze();
		break;
	}
	case CS_PACKET_MATCHING: {
		MATCHING(Id);
		break;
	}
	case CS_PACKET_SERVER_LOGIN: {
		Connect_Socket();
		Send_Server_Login_Ok();
		break;
	}
	default:
		cout << " ������ŶŸ��" << packet_type << endl;
		printf("Unknown PACKET type\n");
		break;
	}
}


void PManager::ProcessServerPacket(int Id,unsigned char* recv)
{
	IdNum = Id;
	RecvBuf = recv;;
	b_server = &BattleServers[IdNum];
	RecvBuf = recv;;
	unsigned char packet_type = recv[1];

	switch (packet_type) {
	case SS_PACKET_SERVER_LOGIN: {
		BattleServers[IdNum].s_id = IdNum;
		break;
	}
	case SS_PACKET_SERVER_LOGIN_OK: {

		break;
	}
	case SS_PACKET_SERVER_RESTART: {
		Server_Restart();
		break;
	}
	default:
		cout << " ������ŶŸ��" << packet_type << endl;
		printf("Unknown PACKET type\n");
		break;
	}
};

bool PManager::Login()
{
	cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(RecvBuf);

	bool bOverlap = false;
	if (strcmp("Tornado", packet->id) != 0) {
		if (strcmp("testuser", packet->id) != 0)
		{

			printf_s("[Recv login] ID : %s, PASSWORD : %s, z : %f\n", packet->id, packet->pw, packet->z);

			for (int i = 0; i < MAX_USER; ++i) {
				clients[i].state_lock.lock();
				if (ST_INGAME == clients[i].cl_state) {
					if (strcmp(packet->id, clients[i]._id) == 0) {
						cout << packet->id << "�������� �÷��̾�" << endl;
						send_login_fail_packet(IdNum, OVERLAP_AC);
						bOverlap = true;
						clients[i].state_lock.unlock();
						break;
					}
				}
				clients[i].state_lock.unlock();
			}

			if (bOverlap == true) return false;
			if (DB_Login(packet->id, packet->pw, cl->LogInInfo) == true)
			{
				strcpy_s(cl->_id, packet->id);
				SetPos(IdNum, packet->id, packet->pw, packet->z);
				cl->color = GA.g_color++;
				printf(" [DB_odbc] �÷��̾�[id] - ID : %s �α��� ���� �÷� %d \n", cl->_id, cl->color);
				send_login_ok_packet(IdNum);
				SendPlayerInfo(IdNum);
			}
			else
			{
				if (DB_Check_Id(packet->id) == true) {
					cout << "�÷��̾�[" << IdNum << "]" << " �߸��� ���" << endl;
					send_login_fail_packet(IdNum, WORNG_PW);
					return false;
				}
				else {
					cout << "�÷��̾�[" << IdNum << "]" << " �߸��� ���̵�" << endl;
					send_login_fail_packet(IdNum, WORNG_ID);
					return false;
				}
			}
		}
		else
		{
			string gm_id(packet->id);
			gm_id += to_string(IdNum);
			SetPos(IdNum, (char*)gm_id.c_str(), packet->pw, packet->z);
			cl->color = GA.g_color++;
			printf(" [GM] �÷��̾�[id] - ID : %s �α��� ���� \n", cl->_id);
			send_login_ok_packet(IdNum);
			SendPlayerInfo(IdNum);
		}
	}
	else
	{
		GA.g_tonardo = true;
		cl->pl_state = ST_TORNADO;
		send_login_ok_packet(IdNum);
		//cout << "����̵�" << endl;
	}
	return true;

};

bool PManager::CreateAccount()
{
	cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(RecvBuf);
	bool _OverLap = false;
	if (strcmp("Tornado", packet->id) != 0) {

		printf_s("[Recv login] ID : %s, PASSWORD : %s\n", packet->id, packet->pw);
		if (_OverLap == true) return false;
		if (DB_Check_Id(packet->id) == true) {
			cout << "�÷��̾�[" << IdNum << "] ���� ���� ���� - " << "�ߺ��� ���̵�" << endl;
			send_login_fail_packet(IdNum, OVERLAP_ID);
		}
		else
		{	//DB�� ������� 
			strcpy_s(cl->_id, packet->id);
			strcpy_s(cl->_pw, packet->pw);
			DB_SignUp(IdNum);
			send_login_fail_packet(IdNum, CREATE_AC);
			cout << packet->id << "DB�� �������" << endl;
		}
	}
	return true;
};

void PManager::Move(int type)
{
	switch (type)
	{
	case CS_PACKET_MOVE:
	{
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(RecvBuf);
		cl->x = packet->x;
		cl->y = packet->y;
		cl->z = packet->z;
		cl->Yaw = packet->yaw;
		cl->VX = packet->vx;
		cl->VY = packet->vy;
		cl->VZ = packet->vz;
		cl->direction = packet->direction;

		for (auto& other : clients) {
			if (other._s_id == IdNum)
				continue;
			if (ST_INGAME != other.cl_state)
				continue;
			send_move_packet(other._s_id, cl->_s_id);
		}
		break;
	}
	case CS_PACKET_NPC_MOVE:
	{
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(RecvBuf);
		if (GA.g_start_game) {
			// cout << "����̵� move" << endl;
			for (auto& other : clients) {
				if (other._s_id == IdNum)
					continue;
				if (ST_INGAME != other.cl_state)
					continue;
				if (ST_TORNADO == other.pl_state)
					continue;
				packet->type = SC_PACKET_NPC_MOVE;
				other.do_send(sizeof(*packet), packet);
			}
		}
		else {
			clients[packet->sessionID]._s_id = packet->sessionID;
			clients[packet->sessionID].x = packet->x;
			clients[packet->sessionID].y = packet->y;
			clients[packet->sessionID].z = packet->z;
			clients[packet->sessionID].VX = packet->vx;
			clients[packet->sessionID].VY = packet->vy;
			clients[packet->sessionID].VZ = packet->vz;
		}
		break;
	}
	default:
		break;
	}
};

void PManager::Attack(int type)
{
	switch (type)
	{
	case HAND:
	{
		cs_packet_attack* packet = reinterpret_cast<cs_packet_attack*>(RecvBuf);
		for (auto& other : clients) {
			if (ST_INGAME != other.cl_state)
				continue;
			packet->type = SC_PACKET_ATTACK;
			other.do_send(sizeof(*packet), packet);
		}
		cout << "attack " << packet->bullet << endl;
		break;
	}
	case GUN:
	{
		cs_packet_attack* packet = reinterpret_cast<cs_packet_attack*>(RecvBuf);
		for (auto& other : clients) {
			if (ST_INGAME != other.cl_state)
				continue;
			packet->type = SC_PACKET_GUNATTACK;
			other.do_send(sizeof(*packet), packet);
		}
		printf("gunattack\n");
		break;
	}
	default:
		break;
	}

};

bool PManager::Damage()
{
	cs_packet_damage* packet = reinterpret_cast<cs_packet_damage*>(RecvBuf);
	if (cl->bIsSnowman) return false;
	cout << "�÷��̾� " << cl->_s_id << "������ ���� " << endl;
	int current_hp = cl->_hp;
	cl->_hp -= 30;
	if (cl->_hp < 270) cl->_hp = 270;
	send_hp_packet(cl->_s_id);

	if (current_hp == cl->_max_hp && cl->is_bone == true)
	{
		cl->_is_active = true;
		Heal(IdNum, BONFIRE);
	}

	if (cl->_hp <= cl->_min_hp)
	{
		cl->_hp = cl->_min_hp;
		cl->iCurrentSnowballCount = 0;
		cl->iCurrentIceballCount = 0;
		cl->iCurrentMatchCount = 0;
		cl->iMaxSnowballCount = cl->iOriginMaxSnowballCount;
		cl->iMaxIceballCount = cl->iOriginMaxIceballCount;
		cl->iMaxMatchCount = cl->iOriginMaxMatchCount;
		cl->bHasBag = false;
		cl->bHasShotGun = false;
		cl->bHasUmbrella = false;
		cl->bIsSnowman = true;

		sc_packet_status_change _packet;
		_packet.size = sizeof(_packet);
		_packet.type = SC_PACKET_STATUS_CHANGE;
		_packet.state = ST_SNOWMAN;
		_packet.s_id = IdNum;
		for (auto& other : clients) {
			if (ST_INGAME != other.cl_state)
				continue;
			other.do_send(sizeof(_packet), &_packet);
			if (packet->bullet == BULLET_SNOWBALL)
				send_kill_log(other._s_id, packet->attacker, cl->_s_id, DeathBySnowball);
			else if (packet->bullet == BULLET_SNOWBOMB)
				send_kill_log(other._s_id, packet->attacker, cl->_s_id, DeathBySnowballBomb);
		}
		cout << "�÷��̾�" << packet->attacker << "�� �����̷� �÷��̾� " << cl->_s_id << "�� ��������� ����" << endl;
		int cnt = 0;
		int target_s_id;
		for (auto& other : clients) {
			if (cl->_s_id == other._s_id) continue;
			if (ST_INGAME != other.cl_state) continue;
			if (false == other.bIsSnowman)
			{
				cnt++;
				//cout << "������ cnt" << endl;

				target_s_id = other._s_id;
			}
		}
		if (cnt == 1) {
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				send_game_end(target_s_id, other._s_id);
			}
			cout << "��������" << endl;
		}
		else {
			int bear_cnt = 0;
			int snowman_cnt = 0;;
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state) continue;
				if (false == other.bIsSnowman)
				{
					bear_cnt++;
				}
				else if (true == other.bIsSnowman)
				{
					snowman_cnt++;
				}
			}
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				send_player_count(other._s_id, bear_cnt, snowman_cnt);
			}
		}
	}
	return true;
};

void PManager::Heal(int id, int type)
{
	switch (type)
	{
	case BONFIRE:
	{
		if (false == clients[id].bIsSnowman) {
			if (clients[id]._hp < clients[id]._max_hp) {
				TimerEvent(id, id, CL_BONEFIRE, 1000ms);
			}
		}
		break;
	}
	case MATCH:
	{
		if (cl->bIsSnowman) break;
		if (cl->iCurrentMatchCount > 0)
		{
			cl->iCurrentMatchCount--;
			if (cl->_hp + 30 > cl->_max_hp)
				cl->_hp = cl->_max_hp;
			else
				cl->_hp += 30;
			send_hp_packet(cl->_s_id);
		}

		break;
	}
	default:
		break;
	}

}

void PManager::GetItem()
{
	cs_packet_get_item* packet = reinterpret_cast<cs_packet_get_item*>(RecvBuf);
	int p_s_id = packet->s_id;

	switch (packet->item_type)
	{

	case ITEM_BAG:
	{
		int item_num = packet->destroy_obj_id;
		bool get_item = is_item(item_num);
		if (!cl->bHasBag) {
			cl->iMaxSnowballCount = cl->iBagMaxSnowballCount;	// ������ 10 -> 15 ���� ���� ����
			cl->iMaxIceballCount = cl->iBagMaxIceballCount;	// ���� 10 -> 15 ���� ���� ����
			cl->iMaxMatchCount = cl->iBagMaxMatchCount;	// ���� 2 -> 3 ���� ���� ����
			cl->bHasBag = true;
		}
		if (get_item && cl->bHasBag == true) {
			cl->bHasBag = true;
			packet->type = SC_PACKET_GET_ITEM;
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				packet->type = SC_PACKET_GET_ITEM;
				other.do_send(sizeof(*packet), packet);
			}
			cout << "�÷��̾�: [" << cl->_s_id << "] ���� �Ĺ� ����" << endl;
		}

		break;
	}
	case ITEM_UMB:
	{
		int item_num = packet->destroy_obj_id;
		bool get_item = is_item(item_num);
		if (get_item && cl->bHasUmbrella == false) {
			cl->bHasUmbrella = true;
			packet->type = SC_PACKET_GET_ITEM;
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				packet->type = SC_PACKET_GET_ITEM;
				other.do_send(sizeof(*packet), packet);
			}
			cout << "�÷��̾�: [" << cl->_s_id << "] ��� �Ĺ� ����" << endl;
		}


		break;
	}
	case ITEM_JET:
	{
		packet->type = SC_PACKET_GET_ITEM;
		for (auto& other : clients) {
			if (ST_INGAME != other.cl_state) continue;
			if (IdNum == other._s_id) continue;
			packet->type = SC_PACKET_GET_ITEM;
			other.do_send(sizeof(*packet), packet);
		}
		cout << "�÷��̾�: [" << cl->_s_id << "] ��Ʈ��Ű on/off" << endl;

		break;
	}
	case ITEM_MAT:
	{
		int item_num = packet->destroy_obj_id;
		bool get_item = is_item(item_num);
		if (get_item && cl->iMaxMatchCount > cl->iCurrentMatchCount) {
			cl->iCurrentMatchCount++;
			packet->type = SC_PACKET_GET_ITEM;
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				packet->type = SC_PACKET_GET_ITEM;
				other.do_send(sizeof(*packet), packet);
			}
			cout << "�÷��̾�: [" << cl->_s_id << "] ���� �Ĺ� ����" << endl;
		}



		break;
	}
	case ITEM_SNOW:
	{
		int snow_drift_num = packet->destroy_obj_id;
		bool get_snowball = is_snowdrift(snow_drift_num);
		if (get_snowball) {
			if (cl->iMaxSnowballCount >= cl->iCurrentSnowballCount + 5)
				cl->iCurrentSnowballCount += 5;
			else
				cl->iCurrentSnowballCount = cl->iMaxSnowballCount;

			packet->type = SC_PACKET_GET_ITEM;
			packet->current_bullet = cl->iCurrentSnowballCount;
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				packet->type = SC_PACKET_GET_ITEM;
				other.do_send(sizeof(*packet), packet);
			}
			cout << "�÷��̾�: [" << cl->_s_id << "] �������� �Ĺ� ����" << endl;
		}
		break;
	}
	case ITEM_ICE:
	{
		int ice_drift_num = packet->destroy_obj_id;
		bool get_iceball = is_icedrift(ice_drift_num);
		if (get_iceball) {
			if (cl->iMaxIceballCount >= cl->iCurrentIceballCount + 5)
				cl->iCurrentIceballCount += 5;
			else
				cl->iCurrentIceballCount = cl->iMaxIceballCount;

			packet->type = SC_PACKET_GET_ITEM;
			packet->current_bullet = cl->iCurrentIceballCount;
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				packet->type = SC_PACKET_GET_ITEM;
				other.do_send(sizeof(*packet), packet);
			}

			cout << "�÷��̾�: [" << cl->_s_id << "] ���������� �Ĺ� ���� ���� ������" << cl->iCurrentIceballCount << endl;
		}
		break;
	}
	case ITEM_SPBOX:
	{
		int spitem_num = packet->destroy_obj_id;
		bool get_spitem = is_spitem(spitem_num);
		if (get_spitem) {
			cl->iCurrentSnowballCount = cl->iMaxSnowballCount;	// ������ 10 
			cl->iCurrentIceballCount = cl->iMaxIceballCount;	// ���� 10 
			cl->iCurrentMatchCount = cl->iMaxMatchCount;	// ���� 2 
			packet->current_bullet = cl->iMaxIceballCount;
			packet->type = SC_PACKET_GET_ITEM;
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				packet->type = SC_PACKET_GET_ITEM;
				other.do_send(sizeof(*packet), packet);
			}

			cout << "�÷��̾�: [" << cl->_s_id << "] ���� �Ĺ� ���� " << endl;
		}
		else
			//cout << "�÷��̾�: [" << cl->_s_id << "]�������� �Ĺ� ����" << endl;
			break;
	}
	default:
		break;
	}
	//cout << "�÷��̾�[" << s_id << "]�� " << "������ [" << _item_no << "]����" << endl;
};

void PManager::UseItem(int type)
{
	switch (type)
	{
	case ITEM_UMB:
	{
		if (cl->bIsSnowman) break;
		if (cl->bHasUmbrella)
		{
			cs_packet_umb* packet = reinterpret_cast<cs_packet_umb*>(RecvBuf);
			if (!packet->end)
				cout << "�÷��̾� " << cl->_s_id << ": ��� ��� " << endl;
			else
				cout << "�÷��̾� " << cl->_s_id << ": ��� ����" << endl;
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				packet->type = SC_PACKET_UMB;
				other.do_send(sizeof(*packet), packet);
			}
		}
		break;
	}
	default:
		break;
	}

};

bool PManager::ItemBox(int type)
{
	switch (type)
	{
	case CS_PACKET_OPEN_BOX: {
		if (cl->bIsSnowman) return false;
		printf("OPEN_BOX\n");
		cs_packet_open_box* packet = reinterpret_cast<cs_packet_open_box*>(RecvBuf);
		for (auto& other : clients) {
			if (IdNum == other._s_id) continue;
			if (ST_INGAME != other.cl_state) continue;
			packet->type = SC_PACKET_OPEN_BOX;
			other.do_send(sizeof(*packet), packet);
		}
		break;
	}
	case CS_PACKET_PUT_OBJECT: {
		if (cl->bIsSnowman) return false;
		printf("SupplyBOX\n");
		sc_packet_put_object* packet = reinterpret_cast<sc_packet_put_object*>(RecvBuf);
		for (auto& other : clients) {
			if (ST_INGAME != other.cl_state) continue;
			packet->type = SC_PACKET_PUT_OBJECT;
			other.do_send(sizeof(*packet), packet);
		}
		break;
	}
	default:
		break;
	}
	return true;
};

void PManager::Snow(int type)
{
	switch (type)
	{
	case SNOW_CREATE:
	{
		cs_packet_throw_snow* packet = reinterpret_cast<cs_packet_throw_snow*>(RecvBuf);
		switch (packet->bullet)
		{
		case BULLET_SNOWBALL:
		{
			if (cl->iCurrentSnowballCount <= 0) break;

			cout << "throw BULLET_SNOWBALL " << packet->speed << endl;
			cl->iCurrentSnowballCount--;
			break;
		}
		case BULLET_ICEBALL:
		{
			if (cl->iCurrentIceballCount <= 0) break;

			cout << "throw BULLET_ICEBALL " << packet->speed << endl;
			cl->iCurrentIceballCount--;
			break;
		}
		default:
			break;
		}
		for (auto& other : clients) {
			if (ST_INGAME != other.cl_state)
				continue;
			packet->type = SC_PACKET_THROW_SNOW;
			other.do_send(sizeof(*packet), packet);
		}
		break;
	}
	case SNOW_DESTORY:
	{
		cs_packet_cancel_snow* packet = reinterpret_cast<cs_packet_cancel_snow*>(RecvBuf);
		for (auto& other : clients) {
			if (ST_INGAME != other.cl_state)
				continue;
			packet->type = SC_PACKET_CANCEL_SNOW;
			other.do_send(sizeof(*packet), packet);
		}
		break;
	}
	default:
		break;
	}
};

bool PManager::Freeze()
{
	if (cl->bIsSnowman) return false;
	cs_packet_freeze* packet = reinterpret_cast<cs_packet_freeze*>(RecvBuf);
	for (auto& other : clients) {
		if (ST_INGAME != other.cl_state)
			continue;
		packet->type = SC_PACKET_FREEZE;
		other.do_send(sizeof(*packet), packet);
	}
	return true;
};

bool PManager::MATCHING(int id)
{
	for (int i = 0; i < MAX_B_SERVER; i++)
	{
		if (BattleServers[i].Matching())
		{
			//�ش� Ŭ���̾�Ʈ���� ��Ʈ ��ȣ ����
			return true;
		}
	}
	std::cout << "���� �������� ����. ���ο� ���� ����\n";
	for (int i = 0; i < MAX_B_SERVER; i++)
	{
		if (BattleServers[i].create_server())
		{
			//�ش� Ŭ���̾�Ʈ���� ��Ʈ ��ȣ ����
			return true;
		}
	}
	return false;
};

bool PManager::GunFire()
{
	if (cl->iCurrentSnowballCount < 4) return false;
	printf("gunfire\n");
	cl->iCurrentSnowballCount -= 5;
	cs_packet_fire* packet = reinterpret_cast<cs_packet_fire*>(RecvBuf);
	for (auto& other : clients) {
		if (ST_INGAME != other.cl_state)
			continue;
		packet->type = SC_PACKET_GUNFIRE;
		SetBulletPos(packet->rand_int);
		other.do_send(sizeof(*packet), packet);
	}
	return true;
};

void PManager::Ready()
{
	sc_packet_ready _packet;
	_packet.size = sizeof(_packet);
	_packet.type = SC_PACKET_READY;
	_packet.s_id = IdNum;
	cl->b_ready = true;
	for (auto& other : clients) {
		if (IdNum == other._s_id)
			continue;
		if (ST_INGAME != other.cl_state)
			continue;
		other.do_send(sizeof(_packet), &_packet);
	}
	cout << IdNum << "�÷��̾� ����" << endl;

	for (auto& other : clients) {
		if (IdNum == other._s_id)
			continue;
		if (ST_INGAME != other.cl_state)
			continue;
		if (other.b_ready == false)
			return;
	}

	sc_packet_start s_packet;
	s_packet.size = sizeof(s_packet);
	s_packet.type = SC_PACKET_START;

	for (auto& other : clients) {
		if (ST_INGAME != other.cl_state)
			continue;
		other.do_send(sizeof(s_packet), &s_packet);
	}

	int bear_cnt = 0;
	int snowman_cnt = 0;;
	for (auto& other : clients) {
		if (ST_INGAME != other.cl_state) continue;
		if (false == other.bIsSnowman)
		{
			bear_cnt++;
		}
		else if (true == other.bIsSnowman)
		{
			snowman_cnt++;
		}
	}
	for (auto& other : clients) {
		if (ST_INGAME != other.cl_state)
			continue;
		send_player_count(other._s_id, bear_cnt, snowman_cnt);
	}
	SetEvent(g_timer);
	GA.g_start_game = true;
	cout << "���� ��ŸƮ" << endl;
};

bool PManager::SetStaus()
{
	sc_packet_status_change* packet = reinterpret_cast<sc_packet_status_change*>(RecvBuf);

	printf_s("[Recv status change] status : %d\n", packet->state);

	if (packet->state == ST_INBURN)
	{
		if (cl->bIsSnowman) return false;
		if (false == cl->is_bone) {
			cl->is_bone = true;
		}
		cl->_is_active = true;
		Heal(IdNum, BONFIRE);
		cout << IdNum << "�÷��̾� ��ں� ����" << endl;

	}
	else if (packet->state == ST_OUTBURN)
	{
		cout << IdNum << "�÷��̾� ��ں� ��" << endl;
		if (cl->bIsSnowman) return false;
		if (true == cl->is_bone) {
			cl->is_bone = false;

		}
		cl->_is_active = true;
		ColdDamage(cl->_s_id);

	}
	else if (packet->state == ST_SNOWMAN)
	{
		cout << "�÷��̾�" << cl->_s_id << "�� �÷��̾�" << packet->s_id << "�� ������̶� ������" << endl;

		if (false == clients[packet->s_id].bIsSnowman) {
			clients[packet->s_id]._hp = clients[packet->s_id]._min_hp;
			clients[packet->s_id].iCurrentSnowballCount = 0;
			clients[packet->s_id].iCurrentIceballCount = 0;
			clients[packet->s_id].iCurrentMatchCount = 0;
			clients[packet->s_id].iMaxSnowballCount = clients[packet->s_id].iOriginMaxSnowballCount;
			clients[packet->s_id].iMaxIceballCount = clients[packet->s_id].iOriginMaxIceballCount;
			clients[packet->s_id].iMaxMatchCount = clients[packet->s_id].iOriginMaxMatchCount;
			clients[packet->s_id].bHasBag = false;
			clients[packet->s_id].bHasShotGun = false;
			clients[packet->s_id].bHasUmbrella = false;
			clients[packet->s_id].bIsSnowman = true;

			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				send_state_change(packet->s_id, other._s_id, ST_SNOWMAN);
				send_kill_log(other._s_id, cl->_s_id, packet->s_id, DeathBySnowman);
			}

			int cnt = 0;
			int target_s_id;
			for (auto& other : clients) {
				//if (cl->_s_id == other._s_id) continue;
				if (ST_INGAME != other.cl_state) continue;
				if (false == other.bIsSnowman)
				{
					cnt++;
					//cout << "�浹 cnt" << endl;

					target_s_id = other._s_id;
				}
			}
			if (cnt == 1) {
				for (auto& other : clients) {
					if (ST_INGAME != other.cl_state)
						continue;
					send_game_end(target_s_id, other._s_id);
				}
				cout << "��������" << endl;
			}
			else {
				int bear_cnt = 0;
				int snowman_cnt = 0;;
				for (auto& other : clients) {
					if (ST_INGAME != other.cl_state) continue;
					if (false == other.bIsSnowman)
					{
						bear_cnt++;
					}
					else if (true == other.bIsSnowman)
					{
						snowman_cnt++;
					}
				}
				for (auto& other : clients) {
					if (ST_INGAME != other.cl_state)
						continue;
					send_player_count(other._s_id, bear_cnt, snowman_cnt);
				}
			}

		}
		//cout << "�÷��̾�" << packet->s_id <<" �����" << endl;

	}
	else if (packet->state == ST_ANIMAL)
	{
		//cout << "�÷��̾�" << s_id << "�� �÷��̾�" << packet->s_id << "�� �����̶� ������" << endl;
		if (true == clients[packet->s_id].bIsSnowman) {
			clients[packet->s_id]._hp = clients[packet->s_id]._BeginSlowHP;
			clients[packet->s_id].bIsSnowman = false;
			//send_hp_packet(packet->s_id);
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				send_state_change(packet->s_id, other._s_id, ST_ANIMAL);
				//cout << "����ȭ" << endl;
				//	cout <<"������ �÷��̾�" << cl->_s_id << "���� �÷��̾�" << other._s_id << endl;
			}
		}
		//cout << "�÷��̾�" << packet->s_id << " ����" << endl;
	}
	return true;
};

void PManager::SetPos(int s_id, char* _id, char* _pw, float _z)
{
	strcpy_s(cl->_id, _id);
	strcpy_s(cl->_pw, _pw);
	cl->cl_state = ST_INGAME;
	cl->x = 600.0f * cos(s_id + 45.0f);
	cl->y = 600.0f * sin(s_id + 45.0f);
	cl->z = _z;
	cl->Yaw = s_id * 55.0f - 115.0f;
	if (cl->Yaw > 180) cl->Yaw -= 360;
	cl->_hp = cl->_max_hp;
}

//�÷��̾� ���� ���� ����
void PManager::SendPlayerInfo(int _s_id)
{
	// ���� ������ �÷��̾��� ������ ���� �÷��̾�� ������
	for (auto& other : clients) {
		if (other._s_id == _s_id) continue;
		if (ST_TORNADO == other.pl_state) continue;
		other.state_lock.lock();
		if (ST_INGAME != other.cl_state) {
			other.state_lock.unlock();
			continue;
		}
		else other.state_lock.unlock();

		sc_packet_put_object packet;
		packet.s_id = cl->_s_id;
		packet.obj_id = cl->color;
		strcpy_s(packet.name, cl->_id);
		packet.size = sizeof(packet);
		packet.type = SC_PACKET_PUT_OBJECT;
		packet.x = cl->x;
		packet.y = cl->y;
		packet.z = cl->z;
		packet.yaw = cl->Yaw;
		packet.object_type = PLAYER;
		strcpy_s(packet.name, cl->_id);
		other.do_send(sizeof(packet), &packet);
	}

	// ���� ������ �÷��̾�� ���� ��ü ������ ������
	for (auto& other : clients) {
		if (other._s_id == _s_id) continue;
		if (other.pl_state == ST_TORNADO) continue;
		if (ST_INGAME != other.cl_state)continue;
		sc_packet_put_object packet;
		packet.s_id = other._s_id;
		packet.obj_id = other.color;
		strcpy_s(packet.name, other.name);
		packet.size = sizeof(packet);
		packet.type = SC_PACKET_PUT_OBJECT;
		packet.x = other.x;
		packet.y = other.y;
		packet.z = other.z;
		packet.yaw = other.Yaw;
		packet.object_type = PLAYER;
		strcpy_s(packet.name, other._id);
		cl->do_send(sizeof(packet), &packet);
	}
	// ����̵� ����
	if (GA.g_tonardo) {
		int iMAX_USER = MAX_USER;
		static_cast<__int64>(iMAX_USER);
		for (int i = 0; i < 3; i++)
		{
			sc_packet_put_object packet;
			packet.s_id = clients[static_cast<__int64>(i) + iMAX_USER]._s_id;
			packet.size = sizeof(packet);
			packet.type = SC_PACKET_PUT_OBJECT;
			packet.x = clients[static_cast<__int64>(i) + iMAX_USER].x;
			packet.y = clients[static_cast<__int64>(i) + iMAX_USER].y;
			packet.z = clients[static_cast<__int64>(i) + iMAX_USER].z;
			packet.yaw = 0.0f;
			packet.object_type = TONARDO;
			printf_s("[����̵� ����] id : %d, location : (%f,%f,%f), yaw : %f\n", packet.s_id, packet.x, packet.y, packet.z, packet.yaw);
			cl->do_send(sizeof(packet), &packet);
		}
	}
}

void PManager::DelvelopCheat()
{
	cs_packet_chat* packet = reinterpret_cast<cs_packet_chat*>(RecvBuf);
	int p_s_id = packet->s_id;
	switch (packet->cheat_type)
	{
	case CHEAT_HP_UP:
	{
		cout << "�÷��̾� " << cl->_s_id << ": ġƮ 1 ��� " << endl;
		if (cl->_hp + 30 > cl->_max_hp)
			cl->_hp = cl->_max_hp;
		else
			cl->_hp += 30;
		send_hp_packet(cl->_s_id);
		break;
	}
	case CHEAT_HP_DOWN:
	{
		if (cl->bIsSnowman) break;
		cout << "�÷��̾� " << cl->_s_id << "ġƮ 2 ��� " << endl;
		int current_hp = cl->_hp;
		cl->_hp -= 30;
		if (cl->_hp < 270) cl->_hp = 270;
		send_hp_packet(cl->_s_id);

		if (current_hp == cl->_max_hp && cl->is_bone == true)
		{
			cl->_is_active = true;
			Heal(cl->_s_id, BONFIRE);
		}

		if (cl->_hp <= cl->_min_hp)
		{
			cl->iCurrentSnowballCount = 0;
			cl->iCurrentIceballCount = 0;
			cl->iCurrentMatchCount = 0;
			cl->iMaxSnowballCount = cl->iOriginMaxSnowballCount;
			cl->iMaxMatchCount = cl->iOriginMaxMatchCount;
			cl->bHasBag = false;
			cl->bHasUmbrella = false;
			cl->bIsSnowman = true;
			sc_packet_status_change _packet;
			_packet.size = sizeof(_packet);
			_packet.type = SC_PACKET_STATUS_CHANGE;
			_packet.state = ST_SNOWMAN;
			_packet.s_id = IdNum;
			for (auto& other : clients) {
				if (ST_INGAME != other.cl_state)
					continue;
				other.do_send(sizeof(_packet), &_packet);
				//cout << "�����" << endl;
				//	cout <<"������ �÷��̾�" << cl->_s_id << "���� �÷��̾�" << other._s_id << endl;
			}
			int cnt = 0;
			int target_s_id;
			for (auto& other : clients) {
				if (cl->_s_id == other._s_id) continue;
				if (ST_INGAME != other.cl_state) continue;
				if (false == other.bIsSnowman)
				{
					cnt++;
					//cout << "������ cnt" << endl;

					target_s_id = other._s_id;
				}
			}
			if (cnt == 1) {
				for (auto& other : clients) {
					if (ST_INGAME != other.cl_state)
						continue;
					send_game_end(target_s_id, other._s_id);
				}
				cout << "��������" << endl;
			}
		}

		break;
	}
	case CHEAT_SNOW_PLUS:
	{
		cout << "�÷��̾� " << cl->_s_id << "ġƮ 3 ��� " << endl;

		if (cl->iMaxSnowballCount >= cl->iCurrentSnowballCount + 5)
			cl->iCurrentSnowballCount += 5;
		else
			cl->iCurrentSnowballCount = cl->iMaxSnowballCount;
		cs_packet_get_item packet;
		packet.size = sizeof(packet);
		packet.type = SC_PACKET_GET_ITEM;
		packet.s_id = cl->_s_id;
		packet.item_type = ITEM_SNOW;
		packet.destroy_obj_id = -1;
		packet.current_bullet = cl->iCurrentSnowballCount;
		for (auto& other : clients) {
			if (ST_INGAME != other.cl_state) continue;
			other.do_send(sizeof(packet), &packet);
		}
		break;
	}
	case CHEAT_ICE_PLUS:
	{
		cout << "�÷��̾� " << cl->_s_id << "ġƮ 4 ��� " << endl;

		if (cl->iMaxIceballCount >= cl->iCurrentIceballCount + 5)
			cl->iCurrentIceballCount += 5;
		else
			cl->iCurrentIceballCount = cl->iMaxIceballCount;
		cs_packet_get_item packet;
		packet.size = sizeof(packet);
		packet.type = SC_PACKET_GET_ITEM;
		packet.s_id = cl->_s_id;
		packet.item_type = ITEM_ICE;
		packet.destroy_obj_id = -1;
		packet.current_bullet = cl->iCurrentIceballCount;
		for (auto& other : clients) {
			if (ST_INGAME != other.cl_state) continue;
			other.do_send(sizeof(packet), &packet);
		}
		break;
	}
	default:
		break;
	}
};

void PManager::SetBulletPos(int* r_arr) {
	int cnt = 0;
	int test[MAX_BULLET_RANG];
	memset(test, 0, MAX_BULLET_RANG * 4);
	while (cnt < MAX_BULLET_RANG) {
		int32 num = uid(dre);
		if (test[num] == 0) {
			test[num] = 1;
			r_arr[cnt] = num;
			cnt++;
		}
	}
}
void PManager::ColdDamage(int Id)
{
	if (false == clients[Id].bIsSnowman) {
		if (clients[Id]._hp > clients[Id]._min_hp) {
			TimerEvent(Id, Id, CL_BONEOUT, 1000ms);
		}
	}
};

void PManager::PutSupplyBox()
{
	TimerEvent(Gm_id, Gm_id, SP_DROP, 60000ms);
	cout << "supply_box " << endl;

}

//Ÿ�̸� ť ���
void PManager::TimerEvent(int np_s_id, int user_id, int ev, std::chrono::milliseconds ms)
{
	timer_ev order;
	order.this_id = np_s_id;
	order.target_id = user_id;
	order.order = ev;
	order.start_t = chrono::system_clock::now() + ms;
	timer_q.push(order);
	//cout << "Timer_Event  " << ev << endl;

}


int PManager::get_id()
{
	static int g_id = 0;

	for (int i = 0; i < MAX_USER; ++i) {
		clients[i].state_lock.lock();
		if (ST_FREE == clients[i].cl_state) {
			clients[i].cl_state = ST_ACCEPT;
			clients[i].state_lock.unlock();
			return i;
		}
		else clients[i].state_lock.unlock();
	}
	cout << "Maximum Number of Clients Overflow!!\n";
	return -1;
}

int PManager::set_port()
{
	for (int i = 0; i < MAX_B_SERVER; i++)
	{
		SERVER_STATE st;
		BattleServers[i].Get_State(st);
		if (st == SERVER_FREE)
		{
			BattleServers[i].Set_State(SERVER_MATHCING);
		}
	}
	return 0;
};

bool PManager::ServerLogin()
{
	ss_packet_server_login* packet = reinterpret_cast<ss_packet_server_login*>(RecvBuf);
	b_server->Port_Num = packet->port_num;
	b_server->Set_State(SERVER_MATHCING);
	return true;

};

bool PManager::Connect_Socket()
{
	ss_packet_server_login* packet = reinterpret_cast<ss_packet_server_login*>(RecvBuf);
	int s_id = packet->port_num - SERVER_PORT;
	b_server = &BattleServers[s_id];
	b_server->Port_Num = packet->port_num;
	b_server->Set_State(SERVER_MATHCING);
	b_server->Lobby_socket = &cl->_socket;
	cl->cl_state = ST_SERVER;
	b_server->Lobby_do_recv();
	return true;

};

bool PManager::Send_Server_Login_Ok()
{
	ss_packet_server_login_ok _packet;
	_packet.size = sizeof(_packet);
	_packet.type = SS_PACKET_SERVER_LOGIN_OK;
	_packet.server_id = b_server->s_id;
	b_server->Lobby_do_send(sizeof(_packet), &_packet);
	return true;
};

bool PManager::Server_Restart()
{
	b_server->reset();
	return true;
};