// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSocket.h"
#include <sstream>
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include <algorithm>
#include <string>

ClientSocket::ClientSocket()
	:StopTaskCounter(0)
{
}

ClientSocket::~ClientSocket()
{
	delete Thread;
	Thread = nullptr;

	closesocket(_socket);
	WSACleanup();
}

bool ClientSocket::InitSocket()
{

	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	// TCP 소켓 생성	
	_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	
	return true;
}

bool ClientSocket::Connect(const char* pszIP, int nPort)
{
	// 접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int nRet = connect(_socket, (sockaddr*)&server_addr , sizeof(sockaddr));
	if (nRet == SOCKET_ERROR) {
		return false;
	}

	return true;
}

