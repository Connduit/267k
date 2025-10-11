#include "../utils/encryption_util.h"
#include <stdio.h>

int main()
{
    printf("kernel32.dll hash = 0x%08X\n", mult_hash("kernel32.dll"));
    printf("ws2_32.dll hash = 0x%08X\n", mult_hash("ws2_32.dll"));
    printf("ntdll.dll hash = 0x%08X\n", mult_hash("ntdll.dll"));

    printf("VirtualAlloc hash = 0x%08X\n", mult_hash("VirtualAlloc"));
    printf("VirtualProtect hash = 0x%08X\n", mult_hash("VirtualProtect"));
    printf("CreateThread hash = 0x%08X\n", mult_hash("CreateThread"));
    printf("WaitForSingleObject hash = 0x%08X\n", mult_hash("WaitForSingleObject"));
    printf("LdrLoadDll hash = 0x%08X\n", mult_hash("LdrLoadDll"));
    printf("WSAStartup hash = 0x%08X\n", mult_hash("WSAStartup"));
    printf("WSACleanup hash = 0x%08X\n", mult_hash("WSACleanup"));
    printf("getaddrinfo hash = 0x%08X\n", mult_hash("getaddrinfo"));
    printf("freeaddrinfo hash = 0x%08X\n", mult_hash("freeaddrinfo"));
    printf("socket hash = 0x%08X\n", mult_hash("socket"));
    printf("closesocket hash = 0x%08X\n", mult_hash("closesocket"));
    printf("connect hash = 0x%08X\n", mult_hash("connect"));
    printf("recv hash = 0x%08X\n", mult_hash("recv"));
    printf("send hash = 0x%08X\n", mult_hash("send"));

    return 0;
}