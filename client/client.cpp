#include "client.h"
//#include <sys/socket.h> unix
#include <winsock2.h> // windows

#pragma comment(lib, "Ws2_32.lib") // ignored by MinGW but harmless

int main()
{
	SOCKET sock = INVALID_SOCKET;
	// SOCK_RAW would provide more control but is more complicated and requires elevated perms
	// IPPROTO_TCP can be NULL instead? if NULL, kernel chooses protocal... tcp by default
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

	SOCKADDR_IN addr;
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)))
	{
		// throw error
		return 1;
	}

	return 0;
}
