#pragma once

#include "Packetdef.h"

#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>

#include <ctime>
#include <cstdio>
#include <cstdarg>

class CClient
{
public:
	CClient();
	~CClient();

	void Initialize();
	void SendPacket(const void*);
	bool CheckConnectivity() const { return m_Sock != INVALID_SOCKET; }

	int m_ReadCounter;
	int m_ReadCurrent;
	unsigned char m_ReadBuffer[65535];

	ceState m_State;
	SOCKET m_Sock;
	sockaddr_in m_Addr;

	int m_ConnId;
	int m_GameId;

	// Socket functions:
	void CloseSocket();
	void ReadPacket();
};

static void Log(const char* str, ...)
{
	char buf[1024];

	va_list arglist;
	va_start(arglist, str);
	vsprintf(buf, str, arglist);
	va_end(arglist);

	time_t rawnow = time(0);
	tm* now = localtime(&rawnow);

	printf("%02d/%02d/%02d %02d:%02d:%02d %s\n", now->tm_mday, now->tm_mon + 1, now->tm_year - 100, now->tm_hour, now->tm_min, now->tm_sec, buf);
}

