// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include "Final_Project.h"
#include "NetworkData.h"

class AMyGameMode;
class AMyPlayerController;

using namespace std;

class FINAL_PROJECT_API ClientSocket
{
public:
	ClientSocket() {
		WSAData wsaData;
		if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			MYLOG(Warning, TEXT("Failed to start wsa"));

		_socket = ::socket(AF_INET, SOCK_STREAM, 0);
		if (_socket == INVALID_SOCKET)
			MYLOG(Warning, TEXT("Failed to start socket"));

		u_long on = 1;
		if (::ioctlsocket(_socket, FIONBIO, &on) == INVALID_SOCKET)
			MYLOG(Warning, TEXT("Failed to start iostlsocket"));
	};
	~ClientSocket();

	bool Connect();

	void SendPacket(void* packet);
	void ReadyToSend_LoginPacket();
	void ReadyToSend_MovePacket(char dr);

	SOCKET _socket;
	string _name;
};