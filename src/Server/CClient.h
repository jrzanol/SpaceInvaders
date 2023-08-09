#pragma once

#include "Packetdef.h"

#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>

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

	SOCKET m_Sock;

	// Socket functions:
	void CloseSocket();
	void ReadPacket();
};

