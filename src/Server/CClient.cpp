
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
				Log("Cliente %s:%d da sala %d desconectou.", inet_ntoa(m_Addr.sin_addr), ntohs(m_Addr.sin_port), m_GameId);

				closesocket(m_Sock);
				m_Sock = INVALID_SOCKET;
			}
		}
		else
			m_ReadCounter += len;
	}
}

void CClient::SendPacket(const void* packet)
{
	if (m_Sock != INVALID_SOCKET)
	{
		PacketHeader* header = (PacketHeader*)packet;

		int ret = send(m_Sock, (char*)header, header->Size, 0);
		if (ret != header->Size)
			printf("CClient::ProcessMiliSecTimer: Erro ao enviar pacote. ret %d size %d lastError %d.\n", ret, header->Size, WSAGetLastError());
	}
}

