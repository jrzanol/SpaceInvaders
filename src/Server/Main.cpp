
#include "CServer.h"

#include <cstdio>
#include <cstdlib>

int main()
{
	CServer server;
	server.InitializeSocket("127.0.0.1", 8000);

	while (server.ProcessEvent())
		;

	return 0;
}

