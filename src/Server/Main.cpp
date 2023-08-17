
#include "CServer.h"

#include <cstdio>
#include <cstdlib>

int main()
{
	Log("Iniciando o servidor...");

	char ipAddress[32];
	ipAddress[0] = 0;

	FILE* in = fopen("serverIp.txt", "rt");
	if (in)
	{
		fgets(ipAddress, sizeof(ipAddress), in);
		fclose(in);
	}

	CServer server;
	server.InitializeSocket(ipAddress, 8000);

	Log("Escutando em %s:8000.", ipAddress);

	while (server.ProcessEvent())
		Sleep(10);

	return EXIT_SUCCESS;
}

