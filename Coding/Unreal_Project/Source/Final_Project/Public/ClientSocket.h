#pragma once

#include "HoloLens/AllowWindowsPlatformAtomics.h"
#include <chrono>
#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include "HoloLens/HideWindowsPlatformAtomics.h"

#include "Final_Project.h"
#include "NetworkData.h"

class AMyGameModeBase;
class AMyPlayerController;

using namespace std;

//================================================================
// 플레이어 변신 상태 + 모닥불 충돌 여부
//================================================================
enum STATE_Type { ST_SNOWMAN, ST_INBURN, ST_OUTBURN, ST_ANIMAL, ST_TORNADO};

//================================================================
// 멀티쓰레드 상황에서 큐에 동시성을 보장해줌
//================================================================

template<typename T>
class LockQueue
{
public:
	LockQueue() { }

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue&) = delete;
	
	void Push(T value)
	//void Push(int32 value)
	{
		lock_guard<mutex> lock(_mutex);
		_queue.push(std::move(value));
		_condVar.notify_one();
	}
	
	bool TryPop(T& value)
	//bool TryPop(int32& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty())
			return false;

		value = std::move(_queue.front());
		_queue.pop();
		return true;
	}

	void WaitPop(T& value)
	//void WaitPop(int32& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] { return _queue.empty() == false; });
		value = std::move(_queue.front());
		_queue.pop();
	}

	void Clear()
	{
		unique_lock<mutex> lock(_mutex);
		if (_queue.empty() == false)
		{
			queue<T> _empty;
			//queue<int32> _empty;
			swap(_queue, _empty);
		}
	}

	int Size()
	{
		unique_lock<mutex> lock(_mutex);
		return _queue.size();
	}


private:
	std::queue<T> _queue;
	//std::queue<int32> _queue;
	std::mutex _mutex;
	std::condition_variable _condVar;
};

//================================================================
// 플레이어 정보가 담긴 클래스
//================================================================
class cCharacter {
public:
	cCharacter() {};
	~cCharacter() {};

	// 세션 아이디
	int SessionId = -1;
	// 아이디 비번
	char	userId[MAX_NAME_SIZE] = {};
	char	userPw[MAX_NAME_SIZE] = {};
	// 위치
	float X = 0;
	float Y = 0;
	float Z = 0;
	// 회전값
	float Yaw = 0;
	float Pitch = 0;
	float Roll = 0;
	// 속도
	float VX = 0;
	float VY = 0;
	float VZ = 0;

	float direction = 0.0f;
	// 속성
	bool	IsAlive = true;
	int	HealthValue = 390.0f;
	bool	IsAttacking = false;
	//카메라 위치
	float fCX, fCY, fCZ;
	//카메라 방향
	float fCDX, fCDY, fCDZ;
	//카메라 yaw
	float fCYaw, fCPitch, fCRoll;
	//발사 속도
	float fSpeed;

	//눈덩이 위치
	float fSBallX, fSBallY, fSBallZ;
	//아이스볼 위치
	float fIBallX, fIBallY, fIBallZ;


	STATE_Type myState = ST_ANIMAL;
	int iCurrentSnowCount = 0;
	int iCurrentIceCount = 0;
	int iCurrentMatchCount = 0;

	int iMaxSnowCount = 10;
	int iMaxMatchCount = 2;

	bool bStartShotGun = false;
	bool bStartSnowBall = false;
	bool bStartIceBall = false;
	
	bool bEndSnowBall = false;
	bool bEndIceBall = false;
	bool bEndShotGun = false;

	bool bCancelSnowBall = false;
	bool bCancelIceBall = false;

	bool bHasUmb = false;
	bool bStartUmb = false;
	bool bEndUmb = false;
	bool bSetJetSki = false;
	bool bGetSpBox = false;
	bool bGetSpBag = false;

	bool bFreezeHead = false;
	bool bFreezeLHand = false;
	bool bFreezeRHand = false;
	bool bFreezeLLeg = false;
	bool bFreezeRLeg = false;
	bool bFreezeCenter = false;
	



	int iRandBulletArr[MAX_BULLET_RANG] = {};

	int iColor = 0;

	friend ostream& operator<<(ostream& stream, cCharacter& info)
	{
		stream << info.SessionId << endl;
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.VX << endl;
		stream << info.VY << endl;
		stream << info.VZ << endl;
		stream << info.Yaw << endl;
		stream << info.Pitch << endl;
		stream << info.Roll << endl;
		stream << info.IsAlive << endl;
		stream << info.HealthValue << endl;
		stream << info.IsAttacking << endl;

		return stream;
	}

	friend istream& operator>>(istream& stream, cCharacter& info)
	{
		stream >> info.SessionId;
		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;
		stream >> info.VX;
		stream >> info.VY;
		stream >> info.VZ;
		stream >> info.Yaw;
		stream >> info.Pitch;
		stream >> info.Roll;
		stream >> info.IsAlive;
		stream >> info.HealthValue;
		stream >> info.IsAttacking;

		return stream;
	}
};

//================================================================
// 플레이어 직렬화/역직렬화 클래스
// map 컨테이너를 활용하여 플레이어 정보에 접근함
//================================================================
class cCharactersInfo
{
public:
	cCharactersInfo() {};
	~cCharactersInfo() {};

	map<int, cCharacter> players;

	friend ostream& operator<<(ostream& stream, cCharactersInfo& info)
	{
		stream << info.players.size() << endl;
		for (auto& kvp : info.players)
		{
			stream << kvp.first << endl;
			stream << kvp.second << endl;
		}

		return stream;
	}

	friend istream& operator>>(istream& stream, cCharactersInfo& info)
	{
		int nPlayers = 0;
		int SessionId = 0;
		cCharacter Player;
		info.players.clear();

		stream >> nPlayers;
		for (int i = 0; i < nPlayers; i++)
		{
			stream >> SessionId;
			stream >> Player;
			info.players[SessionId] = Player;
		}

		return stream;
	}
};

//================================================================
// 서버와 통신을 담당하는 클래스
// 클라이언트 소켓을 담고 있음
//================================================================
class FINAL_PROJECT_API ClientSocket : public FRunnable
{
public:
	ClientSocket() {
		/*WSAData wsaData;
		if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			MYLOG(Warning, TEXT("Failed to start wsa"));

		_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);*/
		//_login_ok = false;

		/*	if (_socket == INVALID_SOCKET)
				MYLOG(Warning, TEXT("Failed to start socket"));

			u_long on = 1;
			if (::ioctlsocket(_socket, FIONBIO, &on) == INVALID_SOCKET)
				MYLOG(Warning, TEXT("Failed to start iostlsocket"));*/
	};
	~ClientSocket();

	bool Connect();
	void ProcessPacket(unsigned char* ptr);
	void Send_LoginPacket(char* _id, char* _pw);
	void Send_Create_ID_Packet(char* send_id, char* send_pw);
	void Send_StatusPacket(int _state, int s_id);

	void Send_TelePortPacket(int point_num);
	void Send_MovePacket(int s_id, FVector MyLocation, float yaw, FVector MyVelocity, float dir);
	void Send_AttackPacket(int s_id, int bullet);
	void Send_GunAttackPacket(int s_id);

	void Send_ChatPacket(int cheat_num);
	void Send_Throw_Packet(int s_id, FVector BallLocation, FRotator MyRotation, int bullet, float speed);
	void Send_Cancel_Packet(int s_id, int bullet);
	void Send_GunFire_Packet(int s_id, FVector MyLocation, FRotator MyRotation);


	void Send_DamagePacket(int attacker, int bullet);

	void Send_ItemPacket(int item_no, int destroy_obj_id);
	void SendPutObjPacket(char cObjType, int iObjId, FVector ObjLocation, float fYaw);
	void Send_LogoutPacket(const int& s_id);
	void Send_ReadyPacket();
	void Send_MatchPacket(); 
	void Send_OpenBoxPacket(int open_box_id);
	void Send_UmbPacket(bool umb_use);
	void Send_FreezePacket(int s_id, int bobyparts);


	// 플레이어 컨트롤러 세팅
	void SetPlayerController(AMyPlayerController* pPlayerController);
	void process_data(unsigned char* net_buf, size_t io_byte);

	void RecvPacket()
	{

		DWORD recv_flag = 0;
		ZeroMemory(&_recv_over._wsa_over, sizeof(_recv_over._wsa_over));
		_recv_over._wsa_buf.buf = reinterpret_cast<char*>(_recv_over._net_buf + _prev_size);
		_recv_over._wsa_buf.len = sizeof(_recv_over._net_buf) - _prev_size;
		int ret = WSARecv(_socket, &_recv_over._wsa_buf, 1, 0, &recv_flag, &_recv_over._wsa_over, NULL);
		if (SOCKET_ERROR == ret) {
			int error_num = WSAGetLastError();
		}
	};
	void SendPacket(void* packet)
	{
		//MYLOG(Warning, TEXT("Send to Server!"));
		int psize = reinterpret_cast<unsigned char*>(packet)[0];
		Overlap* ex_over = new Overlap(OP_SEND, psize, packet);
		int ret = WSASend(_socket, &ex_over->_wsa_buf, 1, 0, 0, &ex_over->_wsa_over, NULL);
		if (SOCKET_ERROR == ret) {
			int error_num = WSAGetLastError();
			//if (ERROR_IO_PENDING != error_num)
				//error_display(error_num);
		}
	};
	void CloseSocket();

	// FRunnable Thread members	
	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;

	// FRunnable override 함수
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

	// 스레드 시작 및 종료
	bool StartListen();
	void StopListen();


	// 싱글턴 객체 가져오기
	static ClientSocket* GetSingleton()
	{
		static ClientSocket ins;
		return &ins;
	}

	HANDLE h_iocp;
	SOCKET _socket;
	Overlap _recv_over;
	char	_id[MAX_NAME_SIZE];
	char	_pw[MAX_NAME_SIZE];

	string _name;
	int      _prev_size = 0;
	bool     _stop = false;
	bool     _login_ok =false;
	int       my_s_id = -1;


private:
	cCharactersInfo CharactersInfo;		// 캐릭터 정보

	AMyPlayerController* MyPlayerController;	// 플레이어 컨트롤러 정보	
};