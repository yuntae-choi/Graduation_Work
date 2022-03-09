// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ClientSocket.h"

#pragma comment(lib, "ws2_32.lib")

DWORD WINAPI ClientMain(LPVOID arg);
void ProcessPacket(char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_PACKET_LOGIN_OK:
	{


	}
	break;
	case SC_PACKET_LOGIN_FAIL:
	{

	}
	break;

	case SC_PACKET_PUT_OBJECT:
	{

		break;
	}
	case SC_PACKET_MOVE:
	{

		break;
	}

	case SC_PACKET_REMOVE_OBJECT:
	{

		break;
	}

	case SC_PACKET_CHAT:
	{

		break;
	}
	case SC_PACKET_STATUS_CHANGE:
	{
	}

	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}
void process_data(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUF_SIZE];

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}
AMyPlayerController::AMyPlayerController()
{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

DWORD WINAPI ClientMain(LPVOID arg)
{
	ClientSocket cs;
	cs.Connect();

	cs.RecvPacket();
	while (1) {
		int num_byte = cs._recv_over._wsa_buf.len;
		if (num_byte == 0) {
			//cout << "연결종료" << endl;
			//Disconnect(client_id);
			continue;
		}

		int remain_data = num_byte + cs._prev_size;
		unsigned char* packet_start = cs._recv_over._net_buf;
		int packet_size = packet_start[0];

		while (packet_size <= remain_data) {
			process_data((char*)packet_start, packet_size);
			remain_data -= packet_size;
			packet_start += packet_size;
			if (remain_data > 0) packet_size = packet_start[0];
			else break;
		}

		if (0 < remain_data) {
			cs._prev_size = remain_data;
			memcpy(cs._recv_over._net_buf, packet_start, remain_data);
		}
		cs.RecvPacket();
		break;
	}

	return 0;
}
