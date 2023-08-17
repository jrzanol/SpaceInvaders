
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
			if (m_Game[connId / 2].m_Alive[0] || m_Game[connId / 2].m_Alive[1])
				continue; // A Sala já está ocorrendo um jogo.

			if (m_Conn[connId].m_Sock == INVALID_SOCKET)
			{
				m_Conn[connId].Initialize();
				m_Conn[connId].m_Sock = sock;
				m_Conn[connId].m_Addr = connAddr;
				m_Conn[connId].m_ConnId = connId;
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
			time_t rawnow = time(0);

			while (it.m_ReadCurrent < it.m_ReadCounter)
			{ // Processamento de todos os pacotes recebidos.
				PacketHeader* pHeader = (PacketHeader*)&it.m_ReadBuffer[it.m_ReadCurrent];

				if (pHeader->Size > (it.m_ReadCounter - it.m_ReadCurrent))
					break; // O pacote não foi totalmente recebido.

				int checksum = 0;

				for (int i = offsetof(PacketHeader, Code); i < pHeader->Size; ++i)
					checksum += ((unsigned char*)pHeader)[i];

				if (checksum != pHeader->Checksum)
					printf("CServer::ProcessPacket: Invalid checksum. checksum %d checksum %d.\n", checksum, pHeader->Checksum);
				else if (abs(rawnow - pHeader->Timestamp) > 5)
					printf("CClient::ProcessPacket: Delayed packet.\n");
				else
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
	if (header->Code == CODE_MSG_RequestInitializeGame)
	{ // Inicia um novo jogo.
		int gameId = conn.m_GameId;
		int connId1 = conn.m_ConnId;
		int connId2 = conn.m_ConnId;

		if ((connId1 % 2) == 1)
			connId1--;
		else
			connId2++;

		if ((m_Conn[connId1].CheckConnectivity() ^ m_Conn[connId2].CheckConnectivity()) != 0)
		{ // Reagrupa as salas, caso estiver somente com 1 jogador.
			int otherGameId;
			for (otherGameId = 0; otherGameId < MAX_CONN / 2; ++otherGameId)
			{
				if (gameId == otherGameId)
					continue;

				if ((m_Conn[otherGameId].CheckConnectivity() ^ m_Conn[otherGameId + 1].CheckConnectivity()) != 0)
				{
					if (!m_Conn[connId1].CheckConnectivity())
					{
						if (m_Conn[otherGameId].CheckConnectivity())
							std::swap(m_Conn[connId1], m_Conn[otherGameId]);
						else
							std::swap(m_Conn[connId1], m_Conn[otherGameId + 1]);

						m_Conn[connId1].m_ConnId = connId1;
						m_Conn[connId1].m_GameId = gameId;
					}
					else
					{
						if (m_Conn[otherGameId].CheckConnectivity())
							std::swap(m_Conn[connId2], m_Conn[otherGameId]);
						else
							std::swap(m_Conn[connId2], m_Conn[otherGameId + 1]);

						m_Conn[connId2].m_ConnId = connId2;
						m_Conn[connId2].m_GameId = gameId;
					}
				}
			}
		}

		m_Game[gameId].m_Seeder = _time32(0);

		MSG_InitializeGame ig;
		ig.Header.Size = sizeof(MSG_InitializeGame);
		ig.Header.Code = CODE_MSG_InitializeGame;
		ig.m_Seeder = m_Game[gameId].m_Seeder;
		ig.m_Count = 0;

		if (m_Conn[connId1].CheckConnectivity())
			ig.m_Count++;

		if (m_Conn[connId2].CheckConnectivity())
			ig.m_Count++;

		if (m_Conn[connId1].CheckConnectivity())
		{
			ig.Header.ID = connId1;
			ig.Header.State = m_Conn[connId1].m_State;

			m_Game[gameId].m_Alive[0] = true;
			m_Conn[connId1].SendPacket(&ig);
		}

		if (m_Conn[connId2].CheckConnectivity())
		{
			ig.Header.ID = connId2;
			ig.Header.State = m_Conn[connId2].m_State;
			
			m_Game[gameId].m_Alive[1] = true;
			m_Conn[connId2].SendPacket(&ig);
			m_Conn[connId2].m_State = ceState::Playing;
		}

		Log("Novo jogo gerado na sala %d, com seeder %d.", gameId, m_Game[gameId].m_Seeder);
	}
	else if (header->Code == CODE_MSG_Action)
	{
		MSG_Action* aa = (MSG_Action*)header;

		int otherConnId = conn.m_ConnId;
		if ((otherConnId % 2) == 1)
			otherConnId--;
		else
			otherConnId++;

		aa->Header.ID = conn.m_ConnId;
		aa->Header.State = conn.m_State;
		m_Conn[otherConnId].SendPacket(aa);
	}
	else if (header->Code == CODE_MSG_Dead)
	{
		MSG_Dead* dd = (MSG_Dead*)header;
		dd->Header.ID = conn.m_ConnId;

		int otherConnId = conn.m_ConnId;
		if ((otherConnId % 2) == 1)
			otherConnId--;
		else
			otherConnId++;

		conn.m_State = ceState::Dead;
		dd->Header.State = conn.m_State;

		m_Game[conn.m_GameId].m_Alive[conn.m_ConnId % 2] = false;

		if (!m_Game[conn.m_GameId].m_Alive[0] && !m_Game[conn.m_GameId].m_Alive[1])
			dd->m_AllIsDead = true;
		else
			dd->m_AllIsDead = false;

		m_Conn[otherConnId].SendPacket(dd);
		m_Conn[conn.m_ConnId].SendPacket(dd);
	}
	else if (header->Code == CODE_MSG_Attack)
	{
		MSG_Attack* aa = (MSG_Attack*)header;
		aa->Header.ID = conn.m_ConnId;

		int otherConnId = conn.m_ConnId;
		if ((otherConnId % 2) == 1)
			otherConnId--;
		else
			otherConnId++;

		aa->Header.State = conn.m_State;
		m_Conn[otherConnId].SendPacket(aa);
		m_Conn[conn.m_ConnId].SendPacket(aa);
	}
}

void CServer::ProcessMiliSecTimer()
{
	for (int gameId = 0; gameId < (MAX_CONN / 2); ++gameId)
	{
		const int connId1 = gameId * 2;
		const int connId2 = gameId * 2 + 1;

		if (m_Conn[connId1].CheckConnectivity() || m_Conn[connId2].CheckConnectivity())
		{
		}
		else
			memset(&m_Game[gameId], 0, sizeof(stGame));
	}
}

