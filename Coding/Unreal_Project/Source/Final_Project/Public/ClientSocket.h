#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include "Final_Project.h"
#include "NetworkData.h"

class AMyGameMode;
class AMyPlayerController;

using namespace std;


// 플레이어 정보
class cCharacter {
public:
	cCharacter() {};
	~cCharacter() {};

	// 세션 아이디
	int SessionId=-1;
	// 위치
	float X=0;
	float Y=0;
	float Z=0;
	// 회전값
	float Yaw=0;
	float Pitch=0;
	float Roll=0;
	// 속도
	float VX;
	float VY;
	float VZ;
	// 속성
	bool	IsAlive;
	float	HealthValue;
	bool	IsAttacking;

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


// 플레이어 직렬화/역직렬화 클래스
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

class FINAL_PROJECT_API ClientSocket : public FRunnable
{
public:
	ClientSocket() {
		WSAData wsaData;
		if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			MYLOG(Warning, TEXT("Failed to start wsa"));

		_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
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
	void ReadyToSend_LoginPacket();
	void ReadyToSend_StatusPacket();
	void ReadyToSend_MovePacket(int s_id, FVector MyLocation, FRotator MyRotation, FVector MyVelocity);
	void ReadyToSend_AttackPacket();
	void ReadyToSend_ChatPacket(int sessionID, float x, float y, float z);
	void ReadyToSend_Throw_Packet(int s_id, FVector MyLocation, FVector MyDirection);

	// 플레이어 컨트롤러 세팅
	void SetPlayerController(AMyPlayerController* pPlayerController);


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
	float	fMy_x;
	float	fMy_y;
	float	fMy_z;

	string _name;
	int      _prev_size = 0;
	bool _stop = false;
	bool _login_ok;

private:

	cCharactersInfo CharactersInfo;		// 캐릭터 정보
	cCharactersInfo* RecvCharacterInfo(stringstream& RecvStream);

	cCharacter	NewPlayer;
	cCharacter* RecvNewPlayer(stringstream& RecvStream);

	AMyPlayerController* PlayerController;	// 플레이어 컨트롤러 정보	
};