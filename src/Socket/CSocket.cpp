
#include "stdafx.h"
#include "CSocket.h"

CSocket::CSocket()
{
	m_ReadCurrent = 0;
	m_ReadCounter = 0;
	m_WriteCounter = 0;

	memset(m_ReadBuffer, 0, sizeof(m_ReadBuffer));
	memset(m_WriteBuffer, 0, sizeof(m_WriteBuffer));

	m_Sock = INVALID_SOCKET;
}

CSocket::~CSocket()
{
}

bool CSocket::InitializeSocket(const char* ip, unsigned short port)
{
	static bool m_StaticInitWsa = false;
	if (!m_StaticInitWsa)
	{
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(1, 1);

		if (WSAStartup(wVersionRequested, &wsaData) != 0)
		{
			printf("CSocket::CSocket: WinSock nao foi iniciado com sucesso.\n");
			exit(0);
		}

		if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
		{
			printf("CSocket::CSocket: WinSock deve estar na versão 1.1 ou superior!\n");
			exit(0);
		}

		m_StaticInitWsa = true;
	}

	m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Sock == INVALID_SOCKET)
	{
		printf("CSocket::CSocket: Falha ao criar o socket.\n");
		return false;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &addr.sin_addr);
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

	if (connect(m_Sock, (sockaddr*)&addr, sizeof(sockaddr_in)) == -1)
	{
		printf("CSocket::CSocket: Falha ao conectar o cliente.\n");

		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
		return false;
	}

	unsigned long nonBlocking = 1;

	if (ioctlsocket(m_Sock, FIONBIO, &nonBlocking) != 0)
	{
		printf("CSocket::CSocket: Falha ao setar o socket para o modo non-blocking.\n");

		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
		return false;
	}

	return true;
}

void CSocket::CloseSocket()
{
	if (m_Sock != INVALID_SOCKET)
	{
		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
	}
}

void CSocket::ReadPacket()
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

void CSocket::ProcessPacket()
{
	if (m_Sock != INVALID_SOCKET && m_ReadCounter > 0)
	{
		time_t rawnow = time(0);

		while (m_ReadCurrent < m_ReadCounter)
		{ // Processamento de todos os pacotes recebidos.
			PacketHeader* pHeader = (PacketHeader*)&m_ReadBuffer[m_ReadCurrent];

			if (pHeader->Size > (m_ReadCounter - m_ReadCurrent))
				break; // O pacote não foi totalmente recebido.

			int checksum = 0;

			for (int i = offsetof(PacketHeader, Code); i < pHeader->Size; ++i)
				checksum += ((unsigned char*)pHeader)[i];

			if (checksum != pHeader->Checksum)
				printf("CClient::ProcessPacket: Invalid checksum. checksum %d checksum %d.\n", checksum, pHeader->Checksum);
			else if (abs(rawnow - pHeader->Timestamp) > 5)
				printf("CClient::ProcessPacket: Delayed packet.\n");
			else
				Process(pHeader);

			m_ReadCurrent += pHeader->Size;
		}

		if (m_ReadCurrent == m_ReadCounter)
		{ // Todos os pacotes foram lidos.
			m_ReadCurrent = 0;
			m_ReadCounter = 0;
		}
	}
}

void CSocket::WritePacket()
{
	if (m_Sock != INVALID_SOCKET && m_WriteCounter > 0)
	{
		int ret = send(m_Sock, (char*)m_WriteBuffer, m_WriteCounter, 0);
		if (ret != m_WriteCounter)
			printf("CClient::ProcessMiliSecTimer: Erro ao enviar pacote. size %d lastError %d.\n", m_WriteCounter, WSAGetLastError());

		m_WriteCounter = 0;
	}
}

void CSocket::SendPacket(const void* packet)
{
	PacketHeader* header = (PacketHeader*)packet;
	header->Timestamp = time(0);
	header->Checksum = 0;

	for (int i = offsetof(PacketHeader, Code); i < header->Size; ++i)
		header->Checksum += ((unsigned char*)packet)[i];

	memcpy(&m_WriteBuffer[m_WriteCounter], (const unsigned char*)header, header->Size);
	m_WriteCounter += header->Size;

	WritePacket();
}

