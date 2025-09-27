#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char **argv) 
{
    if (argc != 2) {
        printf("Usage: %s server-name\n", argv[0]);
        return 1;
    }

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
#endif

    socket_t ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    const char *sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iResult;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve server address
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", WSAGetLastError());
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // Attempt to connect
    for(ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %d\n", WSAGetLastError());
#ifdef _WIN32
            WSACleanup();
#endif
            return 1;
        }

        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            CLOSESOCKET(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // Send data
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        CLOSESOCKET(ConnectSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    printf("Bytes Sent: %d\n", iResult);

    // Shutdown send
    iResult = shutdown(ConnectSocket, SHUTDOWN_WR);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        CLOSESOCKET(ConnectSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // Receive until the server closes the connection
    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
    } while (iResult > 0);

    CLOSESOCKET(ConnectSocket);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
