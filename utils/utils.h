
/*
#include <windows.h>
#include <winternl.h> // TODO: needed for PPEB
HMODULE findModule(const wchar_t* module_name)
{
	PPEB PebAddress;

// get PEB
#if defined(_WIN64)
	PebAddress = (PPEB) __readgsqword(0x60);
#else
	PebAddress = (PPEB) __readfsdword(0x30);
#endif

	// find kernel32.dll
	//PPEB_LDR_DATA ldr = PebAddress->Ldr;
	PLIST_ENTRY list = PebAddress->Ldr->InMemoryOrderModuleList.Flink;

	while (list != &PebAddress->Ldr->InMemoryOrderModuleList)
	{
		// BaseDllName is at offset 0x58 in x64 Windows 10
		UNICODE_STRING *base_dll_name = (UNICODE_STRING *)((BYTE *)list + 0x58);
		PVOID dll_base = *(PVOID *)((BYTE *)list + 0x30);

		if (base_dll_name->Buffer && _wcsicmp(base_dll_name->Buffer, module_name) == 0)
		{
			return (HMODULE)dll_base;
		}
		list = list->Flink;
	}
	return NULL;
}

FARPROC GetProcAddressManual(HMODULE hModule, LPCSTR lpProcName)
{
	PPEB PebAddress;

// get PEB
#if defined(_WIN64)
	PebAddress = (PPEB) __readgsqword(0x60);
#else
	PebAddress = (PPEB) __readfsdword(0x30);
#endif

	// find kernel32.dll
}*/
#ifndef _PAYLOAD_UTIL
#define _PAYLOAD_UTIL

#include <windows.h>
#include <winternl.h>
#include <string.h>

// Redefine PEB structures
typedef struct _MY_PEB_LDR_DATA {
  ULONG Length;
    BOOL Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
  LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
} MY_PEB_LDR_DATA, *PMY_PEB_LDR_DATA;

typedef struct _MY_LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
} MY_LDR_DATA_TABLE_ENTRY, *PMY_LDR_DATA_TABLE_ENTRY;
HMODULE GetProcAddressManual( _In_ LPCSTR lpModuleName, _In_ LPCSTR lpProcName )
{
    PPEB PebAddress;
    PMY_PEB_LDR_DATA pLdr;
    PMY_LDR_DATA_TABLE_ENTRY pDataTableEntry;
    PVOID pModuleBase;
    PIMAGE_DOS_HEADER pDosHeader;
    PIMAGE_NT_HEADERS pNTHeader;
    DWORD dwExportDirRVA;
    PIMAGE_EXPORT_DIRECTORY pExportDir;
    PLIST_ENTRY pNextModule;
    DWORD dwNumFunctions;
    USHORT usOrdinalTableIndex;
    PDWORD pdwFunctionNameBase;
    PCSTR pFunctionName;
    UNICODE_STRING BaseDllName;
    DWORD i;

    // Get PEB
#if defined(_WIN64)
    PebAddress = (PPEB) __readgsqword( 0x60 );
#else
    PebAddress = (PPEB) __readfsdword( 0x30 );
#endif

    pLdr = (PMY_PEB_LDR_DATA) PebAddress->Ldr;
    pNextModule = pLdr->InLoadOrderModuleList.Flink;
    pDataTableEntry = (PMY_LDR_DATA_TABLE_ENTRY) pNextModule;

    while (pDataTableEntry->DllBase != NULL)
    {
        pModuleBase = pDataTableEntry->DllBase;
        BaseDllName = pDataTableEntry->BaseDllName;
        
        // Parse PE headers
        pDosHeader = (PIMAGE_DOS_HEADER)pModuleBase;
        if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            pDataTableEntry = (PMY_LDR_DATA_TABLE_ENTRY)pDataTableEntry->InLoadOrderLinks.Flink;
            continue;
        }
        
        pNTHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)pModuleBase + pDosHeader->e_lfanew);
        if (pNTHeader->Signature != IMAGE_NT_SIGNATURE) {
            pDataTableEntry = (PMY_LDR_DATA_TABLE_ENTRY)pDataTableEntry->InLoadOrderLinks.Flink;
            continue;
        }
        
        dwExportDirRVA = pNTHeader->OptionalHeader.DataDirectory[0].VirtualAddress;

        // Get the next module
        pDataTableEntry = (PMY_LDR_DATA_TABLE_ENTRY)pDataTableEntry->InLoadOrderLinks.Flink;

        // Skip modules without exports
        if (dwExportDirRVA == 0)
            continue;

        // Check module name
        BOOL bModuleMatch = TRUE;
        PCWSTR pModuleNameW = BaseDllName.Buffer;
        LPCSTR pTargetModule = lpModuleName;
        
        for (i = 0; i < BaseDllName.Length / 2; i++)
        {
            WCHAR wc = pModuleNameW[i];
            CHAR c = pTargetModule[i];
            
            if (c == 0) break;
            
            // Simple case-insensitive compare
            if (wc >= L'a' && wc <= L'z') wc -= 32;
            if (c >= 'a' && c <= 'z') c -= 32;
            
            if (wc != c)
            {
                bModuleMatch = FALSE;
                break;
            }
        }

        // Check if we matched the entire module name
        if (!bModuleMatch || (i < strlen(lpModuleName)))
            continue;

        // Parse export directory
        pExportDir = (PIMAGE_EXPORT_DIRECTORY)((ULONG_PTR)pModuleBase + dwExportDirRVA);
        dwNumFunctions = pExportDir->NumberOfNames;
        pdwFunctionNameBase = (PDWORD)((ULONG_PTR)pModuleBase + pExportDir->AddressOfNames);

        for (i = 0; i < dwNumFunctions; i++)
        {
            pFunctionName = (PCSTR)((ULONG_PTR)pModuleBase + pdwFunctionNameBase[i]);
            
            // Compare function name
            if (strcmp(pFunctionName, lpProcName) == 0)
            {
                PWORD pOrdinals = (PWORD)((ULONG_PTR)pModuleBase + pExportDir->AddressOfNameOrdinals);
                PDWORD pFunctions = (PDWORD)((ULONG_PTR)pModuleBase + pExportDir->AddressOfFunctions);
                
                usOrdinalTableIndex = pOrdinals[i];
                return (HMODULE)((ULONG_PTR)pModuleBase + pFunctions[usOrdinalTableIndex]);
            }
        }
    }

    return NULL;
}
#endif