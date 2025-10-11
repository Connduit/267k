#include "../utils/encryption_util.h"
#include <stdio.h>

int main()
{
    printf("kernel32.dll hash = 0x%08X\n", xor_hash_ci("kernel32.dll"));
    printf("ws2_32.dll hash = 0x%08X\n", xor_hash_ci("ws2_32.dll"));
    printf("ntdll.dll hash = 0x%08X\n", xor_hash_ci("ntdll.dll"));

    printf("VirtualAlloc hash = 0x%08X\n", xor_hash_ci("VirtualAlloc"));
    printf("VirtualProtect hash = 0x%08X\n", xor_hash_ci("VirtualProtect"));
    printf("CreateThread hash = 0x%08X\n", xor_hash_ci("CreateThread"));
    printf("WaitForSingleObject hash = 0x%08X\n", xor_hash_ci("WaitForSingleObject"));
    printf("LdrLoadDll hash = 0x%08X\n", xor_hash_ci("LdrLoadDll"));
    printf("WSAStartup hash = 0x%08X\n", xor_hash_ci("WSAStartup"));
    printf("WSACleanup hash = 0x%08X\n", xor_hash_ci("WSACleanup"));
    printf("getaddrinfo hash = 0x%08X\n", xor_hash_ci("getaddrinfo"));
    printf("freeaddrinfo hash = 0x%08X\n", xor_hash_ci("freeaddrinfo"));
    printf("socket hash = 0x%08X\n", xor_hash_ci("socket"));
    printf("closesocket hash = 0x%08X\n", xor_hash_ci("closesocket"));
    printf("connect hash = 0x%08X\n", xor_hash_ci("connect"));
    printf("recv hash = 0x%08X\n", xor_hash_ci("recv"));
    printf("send hash = 0x%08X\n", xor_hash_ci("send"));

    return 0;
}