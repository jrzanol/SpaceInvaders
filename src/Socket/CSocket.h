#pragma once

#include "stdafx.h"
#include "Packetdef.h"

class CSocket
{
protected:
	CSocket();
	~CSocket();

	void SendPacket(const void*);
	bool CheckConnectivity() const { return m_Sock != INVALID_SOCKET; }

	int m_ReadCounter;
	int m_ReadCurrent;
	unsigned char m_ReadBuffer[65535];

	int m_WriteCounter;
	unsigned char m_WriteBuffer[65535];

	SOCKET m_Sock;

	// Socket functions:
	bool InitializeSocket(const char*, unsigned short);
	void CloseSocket();
	void ReadPacket();
	void WritePacket();
	void ProcessPacket();
	virtual void Process(const PacketHeader*) = 0;
};

