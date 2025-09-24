// winsock_client.cpp
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib") // ignored by MinGW but harmless

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: winsock_client <server-ip> <port>\n";
        return 1;
    }

    const char* server_ip = argv[1];
    const char* port = argv[2];

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    addrinfo hints = {}, *res = nullptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(server_ip, port, &hints, &res) != 0) {
        std::cerr << "getaddrinfo failed\n";
        WSACleanup();
        return 1;
    }

    SOCKET sock = INVALID_SOCKET;
    for (addrinfo* p = res; p != nullptr; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == INVALID_SOCKET) continue;
        if (connect(sock, p->ai_addr, (int)p->ai_addrlen) == 0) break;
        closesocket(sock);
        sock = INVALID_SOCKET;
    }

    freeaddrinfo(res);

    if (sock == INVALID_SOCKET) {
        std::cerr << "Could not connect\n";
        WSACleanup();
        return 1;
    }

    const int BUF_SIZE = 1024;
    char buf[BUF_SIZE];
    int ret;
    while ((ret = recv(sock, buf, BUF_SIZE, 0)) > 0) {
        std::cout.write(buf, ret);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}

