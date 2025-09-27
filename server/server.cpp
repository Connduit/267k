#undef UNICODE
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment (lib, "Ws2_32.lib")
    typedef SOCKET socket_t;
    #define CLOSESOCKET closesocket
    #define SOCK_ERR SOCKET_ERROR
    #define SHUTDOWN_WR SD_SEND
#else
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    typedef int socket_t;
    #define INVALID_SOCKET -1
    #define CLOSESOCKET close
    #define SOCKET_ERROR -1
    #define WSAGetLastError() errno
    #define ZeroMemory(ptr, size) memset(ptr, 0, size)
    #define SHUTDOWN_WR SHUT_WR
#endif

int main(void) 
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
#endif

    socket_t ListenSocket = INVALID_SOCKET;
    socket_t ClientSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve address
    if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &result) != 0) {
        perror("getaddrinfo failed");
        return 1;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        return 1;
    }

    if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        CLOSESOCKET(ListenSocket);
        return 1;
    }

    freeaddrinfo(result);

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        CLOSESOCKET(ListenSocket);
        return 1;
    }

    printf("Waiting for client...\n");
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        CLOSESOCKET(ListenSocket);
        return 1;
    }

    CLOSESOCKET(ListenSocket);

    // Echo loop
    int iResult;
    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                CLOSESOCKET(ClientSocket);
#ifdef _WIN32
                WSACleanup();
#endif
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        } else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            CLOSESOCKET(ClientSocket);
#ifdef _WIN32
            WSACleanup();
#endif
            return 1;
        }
    } while (iResult > 0);

    shutdown(ClientSocket, SHUTDOWN_WR);
    CLOSESOCKET(ClientSocket);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
