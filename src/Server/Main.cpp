
#include "CServer.h"

#include <cstdio>
#include <cstdlib>

int main()
{
	Log("Iniciando o servidor...");

	CServer server;
	server.InitializeSocket("127.0.0.1", 8000);

	Log("Escutando em 127.0.0.1:8000.");

	while (server.ProcessEvent());
	return EXIT_SUCCESS;
}

