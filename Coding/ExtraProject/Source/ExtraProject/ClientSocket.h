// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <queue>
#include <mutex>
//#include "EngineMinimal.h"
#include "ExtraProject.h"
#include "NetworkData.h"

class AExtraProjectGameModeBase;
class AMyPlayerController;

using namespace std;

//================================================================
// 플레이어 변신 상태 + 모닥불 충돌 여부
//================================================================
enum STATE_Type { ST_SNOWMAN, ST_INBURN, ST_OUTBURN, ST_ANIMAL };

//================================================================
// 멀티쓰레드 상황에서 큐에 동시성을 보장해줌
//================================================================


//================================================================
// 서버와 통신을 담당하는 클래스
// 클라이언트 소켓을 담고 있음
//================================================================
class EXTRAPROJECT_API ClientSocket : public FRunnable
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
	void Send_LoginPacket();

	void Send_MovePacket(FVector MyLocation, FVector MyVelocity);

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
	char	_id[MAX_NAME_SIZE] = "Tornado";
	char	_pw[MAX_NAME_SIZE];

	string _name;
	int      _prev_size = 0;
	bool     _stop = false;
	bool     _login_ok = false;
	bool     _start_ok = false;
	int       my_s_id = -1;
private:

	AMyPlayerController* MyPlayerController;	// 플레이어 컨트롤러 정보;	
};
