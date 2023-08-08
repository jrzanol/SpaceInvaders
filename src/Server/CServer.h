#pragma once

#include "CClient.h"
#include "Packetdef.h"

const int MAX_CONN = 8;

class CServer
{
public:
	CServer();
	~CServer();

	bool CheckConnectivity() const { return m_Sock != INVALID_SOCKET; }

	// Socket functions:
	bool InitializeSocket(const char*, unsigned short);
	bool ProcessEvent();
	void CloseSocket();

private:
	SOCKET m_Sock;
	CClient m_Conn[MAX_CONN];

	void ProcessPacket();
	void Process(CClient&, PacketHeader*);
};

