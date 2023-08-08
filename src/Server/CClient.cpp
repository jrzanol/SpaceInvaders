
#include "CClient.h"

#include <cstdio>

CClient::CClient()
{
	Initialize();
}

CClient::~CClient()
{
}

void CClient::Initialize()
{
	m_ReadCurrent = 0;
	m_ReadCounter = 0;

	memset(m_ReadBuffer, 0, sizeof(m_ReadBuffer));

	m_Sock = INVALID_SOCKET;
}

void CClient::CloseSocket()
{
	if (m_Sock != INVALID_SOCKET)
	{
		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
	}
}

void CClient::ReadPacket()
{
	if (m_Sock != INVALID_SOCKET)
	{
		int len = recv(m_Sock, (char*)&m_ReadBuffer[m_ReadCounter], sizeof(m_ReadBuffer) - m_ReadCounter, 0);
		if (len <= 0)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				if (len == 0)
					printf("CClient::ProcessMiliSecTimer: Desconectado pelo servidor.\n");
				else
					printf("CClient::ProcessMiliSecTimer: Erro ao receber pacote. len %d size %d lastError %d.\n", len, m_ReadCounter, WSAGetLastError());

				closesocket(m_Sock);
				m_Sock = INVALID_SOCKET;
			}
		}
		else
			m_ReadCounter += len;
	}
}

void CClient::SendPacket(const void* packet, int size)
{
	if (m_Sock != INVALID_SOCKET)
	{
		int ret = send(m_Sock, (char*)packet, size, 0);
		if (ret != size)
			printf("CClient::ProcessMiliSecTimer: Erro ao enviar pacote. ret %d size %d lastError %d.\n", ret, size, WSAGetLastError());
	}
}

