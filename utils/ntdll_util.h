#ifndef _NTDLL_UTIL
#define _NTDLL_UTIL

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// TODO: put UNICODE_STRING in a common area so it can be shared by utils.h too
typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

#define NTAPI __stdcall
typedef long NTSTATUS;

typedef NTSTATUS (NTAPI *FuncLdrLoadDll)(
    _In_opt_ PWSTR PathToFile,
    _In_opt_ PULONG DllCharacteristics,
    _In_ PUNICODE_STRING DllName, 
    _Out_ PVOID *BaseAddress
);


#endif