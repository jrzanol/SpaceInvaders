
#include "CServer.h"

#include <time.h>
#include <cstdio>
#include <algorithm>

CServer::CServer()
{
	m_Sock = INVALID_SOCKET;

	memset(m_Game, 0, sizeof(m_Game));
}

CServer::~CServer()
{
}

bool CServer::InitializeSocket(const char* ip, unsigned short port)
{
	static bool m_StaticInitWsa = false;
	if (!m_StaticInitWsa)
	{
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(1, 1);

		if (WSAStartup(wVersionRequested, &wsaData) != 0)
		{
			printf("CServer::CServer: WinSock nao foi iniciado com sucesso.\n");
			exit(0);
		}

		if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
		{
			printf("CServer::CServer: WinSock deve estar na versão 1.1 ou superior!\n");
			exit(0);
		}

		m_StaticInitWsa = true;
	}

	m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Sock == INVALID_SOCKET)
	{
		printf("CServer::CServer: Falha ao criar o socket.\n");
		return false;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &addr.sin_addr);
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

	if (bind(m_Sock, (sockaddr*)&addr, sizeof(sockaddr_in)) == -1)
	{
		printf("CServer::CServer: Falha ao reservar o cliente.\n");

		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
		return false;
	}

	if (listen(m_Sock, SOMAXCONN) == -1)
	{
		printf("CServer::CServer: Falha ao escutar o cliente.\n");

		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
		return false;
	}

	unsigned long nonBlocking = 1;

	if (ioctlsocket(m_Sock, FIONBIO, &nonBlocking) != 0)
	{
		printf("CServer::CServer: Falha ao setar o socket para o modo non-blocking.\n");

		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
		return false;
	}

	return true;
}

bool CServer::ProcessEvent()
{
	sockaddr_in connAddr;
	socklen_t connAddrSize = sizeof(sockaddr_in);

	SOCKET sock = accept(m_Sock, (sockaddr*)&connAddr, &connAddrSize);
	if (sock == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("CServer::ProcessEvent: Falha ao aceitar o cliente.\n");

			closesocket(m_Sock);
			m_Sock = INVALID_SOCKET;
			return false;
		}
	}
	else
	{
		for (int connId = 0; connId < MAX_CONN; ++connId)
		{
			if (m_Conn[connId].m_Sock == INVALID_SOCKET)
			{
				m_Conn[connId].Initialize();
				m_Conn[connId].m_Sock = sock;
				m_Conn[connId].m_Addr = connAddr;
				m_Conn[connId].m_GameId = connId / 2;

				Log("Cliente %s:%d na sala %d conectou.", inet_ntoa(connAddr.sin_addr), ntohs(connAddr.sin_port), connId / 2);
				break;
			}
		}
	}

	for (auto& it : m_Conn)
		it.ReadPacket();

	ProcessPacket();
	ProcessMiliSecTimer();
	return true;
}

void CServer::CloseSocket()
{
	if (m_Sock != INVALID_SOCKET)
	{
		closesocket(m_Sock);
		m_Sock = INVALID_SOCKET;
	}
}

void CServer::ProcessPacket()
{
	for (auto& it : m_Conn)
	{
		if (it.m_Sock != INVALID_SOCKET && it.m_ReadCounter > 0)
		{
			while (it.m_ReadCurrent < it.m_ReadCounter)
			{ // Processamento de todos os pacotes recebidos.
				PacketHeader* pHeader = (PacketHeader*)&it.m_ReadBuffer[it.m_ReadCurrent];

				if (pHeader->Size > (it.m_ReadCounter - it.m_ReadCurrent))
					break; // O pacote não foi totalmente recebido.

				Process(it, pHeader);

				it.m_ReadCurrent += pHeader->Size;
			}

			if (it.m_ReadCurrent == it.m_ReadCounter)
			{ // Todos os pacotes foram lidos.
				it.m_ReadCurrent = 0;
				it.m_ReadCounter = 0;
			}
		}
	}
}

void CServer::Process(CClient& conn, PacketHeader* header)
{
}

void CServer::ProcessMiliSecTimer()
{
	for (int gameId = 0; gameId < (MAX_CONN / 2); ++gameId)
	{
		const int connId1 = gameId * 2;
		const int connId2 = gameId * 2 + 1;

		if (m_Conn[connId1].CheckConnectivity() || m_Conn[connId2].CheckConnectivity())
		{
			if (!m_Game[gameId].m_Alive[0] && !m_Game[gameId].m_Alive[1])
			{
				m_Game[gameId].m_Seeder = _time32(0);

				MSG_InitializeGame ig;
				ig.Header.Size = sizeof(MSG_InitializeGame);
				ig.Header.Code = CODE_MSG_InitializeGame;
				ig.Seeder = m_Game[gameId].m_Seeder;

				if (m_Conn[connId1].CheckConnectivity())
				{
					m_Game[gameId].m_Alive[0] = true;
					m_Conn[connId1].SendPacket(&ig);
				}

				if (m_Conn[connId2].CheckConnectivity())
				{
					m_Game[gameId].m_Alive[1] = true;
					m_Conn[connId2].SendPacket(&ig);
				}

				Log("Novo jogo gerado na sala %d, com seeder %d.", gameId, m_Game[gameId].m_Seeder);
			}
		}
		else
			memset(&m_Game[gameId], 0, sizeof(stGame));
	}
}

