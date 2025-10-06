// https://github.com/HavocFramework/Havoc/blob/main/payloads/Demon/src/core/MiniStd.c#L313

// Walking:
// https://struppigel.github.io/WisdomForHedgehogs/Execution%20Environments/PEB%20Walking%20and%20Export%20Parsing/
// https://systemweakness.com/inside-the-peb-how-malware-resolves-apis-without-imports-81f7d03bcfc5
// https://github.com/HavocFramework/Havoc/blob/main/payloads/Shellcode/Source/Win32.c
// https://github.com/HavocFramework/Havoc/blob/main/payloads/Demon/src/core/Win32.c
// https://github.com/rapid7/metasploit-payloads/blob/master/c/meterpreter/source/extensions/unhook/refresh.c
// https://github.com/rapid7/metasploit-framework/blob/master/external/source/exploits/juicypotato/JuicyPotato/ReflectiveLoader.c
// https://github.com/rapid7/metasploit-framework/blob/master/external/source/exploits/rottenpotato/MSFRottenPotato/ReflectiveLoader.c
// https://github.com/rapid7/metasploit-framework/blob/master/data/headers/windows/c_payload_util/payload_util.h
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

// TODO: break up GetProcAddressManual into 2 functions so we don't have to recalculate getModule logic everytime
// meaning we only need to call it once inside stager.c

#ifndef _PAYLOAD_UTIL
#define _PAYLOAD_UTIL

// TODO: remove these includes
//#include <windows.h>
//#include <winternl.h>
//#include <string.h>


// TODO: move this somewhere else?
// Define own types
typedef struct _UNICODE_STRING {
	  USHORT Length;
	    USHORT MaximumLength;
		  PWSTR  Buffer;
		  
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _PEB_LDR_DATA {
	BYTE Reserved1[8];
	PVOID Reserved2[3];
	LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA,*PPEB_LDR_DATA;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS,*PRTL_USER_PROCESS_PARAMETERS;

typedef VOID (NTAPI *PPS_POST_PROCESS_INIT_ROUTINE)(VOID);

typedef struct _PEB {
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[1];
	PVOID Reserved3[2];
	PPEB_LDR_DATA Ldr;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	PVOID Reserved4[3];
	PVOID AtlThunkSListPtr;
	PVOID Reserved5;
	ULONG Reserved6;
	PVOID Reserved7;
	ULONG Reserved8;
	ULONG AtlThunkSListPtr32;
	PVOID Reserved9[45];
	BYTE Reserved10[96];
	PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	BYTE Reserved11[128];
	PVOID Reserved12[1];
	ULONG SessionId;
} PEB,*PPEB;


// TODO: use these PEB structures instead not the ones above
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

// TODO: temp strcmp until i implement hashing
BOOL strings_equal(const char* a, const char* b) {
	int i = 0;
	while (a[i] && b[i] && a[i] == b[i]) i++;
	return (a[i] == b[i]);  // Both should be null terminator
}

// TODO: temp strlen
size_t my_strlen(const char* str) {
	size_t len = 0;
	while (str[len]) len++;
	return len;

}

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
        //if (!bModuleMatch || (i < strlen(lpModuleName)))
		// TODO: 
        if (!bModuleMatch || (i < my_strlen(lpModuleName)))
            continue;

        // Parse export directory
        pExportDir = (PIMAGE_EXPORT_DIRECTORY)((ULONG_PTR)pModuleBase + dwExportDirRVA);
        dwNumFunctions = pExportDir->NumberOfNames;
        pdwFunctionNameBase = (PDWORD)((ULONG_PTR)pModuleBase + pExportDir->AddressOfNames);

        for (i = 0; i < dwNumFunctions; i++)
        {
            pFunctionName = (PCSTR)((ULONG_PTR)pModuleBase + pdwFunctionNameBase[i]);
            
            // Compare function name // TODO: replace strcmp with hash comparison
            //if (strcmp(pFunctionName, lpProcName) == 0)
            if (strings_equal(pFunctionName, lpProcName) == 0)
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
