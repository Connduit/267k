#ifndef _UTIL_H
#define _UTIL_H

#include "ministd.h"
#include "encryption_util.h"
#include <stdio.h>
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>

// TODO: Use offsets to reduce the need to define these LDR structs
// see offsets here: https://struppigel.github.io/WisdomForHedgehogs/Execution%20Environments/PEB%20Walking%20and%20Export%20Parsing/

#if defined(_WIN64)
    #define GET_PEB() (PPEB) __readgsqword(0x60)
#else 
    #define GET_PEB() (PPEB) __readfsdword(0x30)
#endif

/*
typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink; // ptr to LDR_DATA_TABLE_ENTRY
   struct _LIST_ENTRY *Blink; // ptr to LDR_DATA_TABLE_ENTRY
} LIST_ENTRY, *PLIST_ENTRY, *RESTRICTED_POINTER PRLIST_ENTRY;
*/

// TODO: put UNICODE_STRING in a common area so it can be shared by ntdll_util.h too
typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

// https://github.com/HavocFramework/Havoc/blob/main/payloads/DllLdr/Include/Native.h#L173
// https://github.com/reactos/reactos/blob/master/sdk/include/ndk/ldrtypes.h#L140
// https://learn.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-peb_ldr_data
typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks; // PVOID Reserved1[2];
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks; // PVOID Reserved2[2];
    PVOID DllBase; // pointer to the base address of where the DLL/module is loaded
    PVOID EntryPoint;
	ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    PVOID Reserved5[3];
    union
    {
        ULONG CheckSum;
        PVOID Reserved6;
    };
    ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB_LDR_DATA {
    ULONG Length;
    BOOLEAN Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    //PVOID EntryInProgress;
	//#if (NTDDI_VERSION >= NTDDI_WIN7)
    //UCHAR ShutdownInProgress;
    //PVOID ShutdownThreadId;
} PEB_LDR_DATA,*PPEB_LDR_DATA;

// TODO: this struct isn't needed? we only need Ldr anyways... might have to uncomment to avoid using certain #includes tho
// If it matters: https://github.com/HavocFramework/Havoc/blob/main/payloads/DllLdr/Include/Native.h#L41
typedef struct _PEB {
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[1];
	PVOID Reserved3[2];
	PPEB_LDR_DATA Ldr;
    /*
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
    */
} PEB,*PPEB;

/* 64bit
typedef struct _PEB {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[21];
    PPEB_LDR_DATA LoaderData;
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
    BYTE Reserved3[520];
    PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
    BYTE Reserved4[136];
    ULONG SessionId;
} PEB;
*/



// TODO: change so if a param isn't passed hmodule to current exe gets returned instead?
// GetModuleHandleManualW
//HMODULE GetModuleHandleManual(LPCWSTR lpModuleName)
HMODULE GetModuleHandleManualHash(DWORD moduleHash)
{
    PPEB PebAddress = GET_PEB();

    SIZE_T Size = 0;
    CHAR ModuleName[MAX_PATH] = {0};

    PVOID pModule;
	PLIST_ENTRY pListHead = &PebAddress->Ldr->InMemoryOrderModuleList;
	PLIST_ENTRY pList = PebAddress->Ldr->InMemoryOrderModuleList.Flink;
	PLDR_DATA_TABLE_ENTRY pDataTableEntry;

	// Find matching module hash
	while (pList != pListHead)
	{
        // TODO: do CONTAINING_RECORD logic manually 
        pDataTableEntry = CONTAINING_RECORD(pList, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks); 
		
		// will need to do some type conversion
		//if (strcmp(pDataTableEntry->BaseDllName.Buffer, lpModuleName) == 0)
        Size = WCharStringToCharString(ModuleName, pDataTableEntry->BaseDllName.Buffer, pDataTableEntry->BaseDllName.Length);
        //wprintf(L"string = %.*s\n", pDataTableEntry->BaseDllName.Length / sizeof(WCHAR), pDataTableEntry->BaseDllName.Buffer);
        //if (Size > 0 && _stricmp(ModuleName, moduleHash) == 0)
        if (Size > 0 && mult_hash(ModuleName) == moduleHash)
		{
            //printf("found match\n");
			pModule = pDataTableEntry->DllBase;
			break;
		}
		pList = pList->Flink;
	}

    return pModule;

}

FARPROC GetProcAddressManualHash(HMODULE hModule, DWORD procHash)
{
	// Find lpProcName
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS) ((ULONG_PTR) hModule + ((PIMAGE_DOS_HEADER) hModule)->e_lfanew); // pModule + 0x3c (32/64bit)
	DWORD dwExportDirRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress; // pModule + 0x3c + 0x88 (64bit)
	PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY) ((ULONG_PTR) hModule + dwExportDirRVA); // pModule + (pModule + 0x3c)

	DWORD* arrayOfFunctionRVAs = (DWORD *)((ULONG_PTR)hModule + pExportDir->AddressOfFunctions); 		// PDWORD
    DWORD* arrayOfNamesRVAs = (DWORD *)((ULONG_PTR)hModule + pExportDir->AddressOfNames);				// PDWORD
    WORD* arrayOfNameOrdinals = (WORD *)((ULONG_PTR)hModule + pExportDir->AddressOfNameOrdinals); 	// PWORD

    for (DWORD i = 0; i < pExportDir->NumberOfNames; ++i)
    {
        char *prodName = (char *)((ULONG_PTR)hModule + arrayOfNamesRVAs[i]);
        WORD ordinalIndex = arrayOfNameOrdinals[i];
        FARPROC functionAddress = (FARPROC)((ULONG_PTR)hModule + arrayOfFunctionRVAs[ordinalIndex]);

        // TODO: idk if case-insensitive strcmp is needed on here
        // printf("prodName = %s\n", prodName);
        //if (_stricmp(lpProcName, prodName) == 0)
        if (mult_hash(prodName) == procHash)
        {
            return functionAddress;
        }
    }
    return NULL;
}


FARPROC GetProcAddressManualM(PVOID pModule, LPCSTR lpProcName)
{
	// Find lpProcName
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS) ((ULONG_PTR) pModule + ((PIMAGE_DOS_HEADER) pModule)->e_lfanew); // pModule + 0x3c (32/64bit)
	DWORD dwExportDirRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress; // pModule + 0x3c + 0x88 (64bit)
	PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY) ((ULONG_PTR) pModule + dwExportDirRVA); // pModule + (pModule + 0x3c)

	DWORD* arrayOfFunctionRVAs = (DWORD *)((ULONG_PTR)pModule + pExportDir->AddressOfFunctions); 		// PDWORD
    DWORD* arrayOfNamesRVAs = (DWORD *)((ULONG_PTR)pModule + pExportDir->AddressOfNames);				// PDWORD
    WORD* arrayOfNameOrdinals = (WORD *)((ULONG_PTR)pModule + pExportDir->AddressOfNameOrdinals); 	// PWORD

    for (DWORD i = 0; i < pExportDir->NumberOfNames; ++i)
    {
        char *prodName = (char *)((ULONG_PTR)pModule + arrayOfNamesRVAs[i]);
        WORD ordinalIndex = arrayOfNameOrdinals[i];
        FARPROC functionAddress = (FARPROC)((ULONG_PTR)pModule + arrayOfFunctionRVAs[ordinalIndex]);

        // TODO: idk if case-insensitive strcmp is needed on here
        // printf("prodName = %s\n", prodName);
        if (_stricmp(lpProcName, prodName) == 0)
        {
            return functionAddress;
        }
    }
    return NULL;
}


// first find the lpModuleName (exe/dll name), then find the lpProcName (proceduce/function name)
FARPROC GetProcAddressManual(LPCSTR lpModuleName, LPCSTR lpProcName)
{
    // Get PEB
    PPEB PebAddress = GET_PEB();
	/*
	Offset Arithmetic (64Bit ONLY):
		PEB_LDR_DATA ldr = (BYTE *)PebAddress + 0x18;

		// TODO
		PLIST_ENTRY pListHead = PebAddress + 0x18 + 0x20
		PBYTE InMemoryOrderModuleList = ldr + 0x20;
		PebAddress                   gs:0x60

  		+ 0x20 → InMemoryOrderModuleList (LIST_ENTRY)
    	+ 0x00 → Flink pointer → next LIST_ENTRY
    	+ 0x20 → DllBase
    	+ 0x38 → FullDllName (UNICODE_STRING)
        + 0x8 → Buffer pointer
    	+ 0x48 → BaseDllName (UNICODE_STRING)
        + 0x8 → Buffer pointer
		PBYTE listHead = PebAddress + 0x18 + 0x20;  // list head
		PBYTE entry = *(PBYTE*)listHead;            // first Flink

		while(entry != listHead) {
    		PBYTE dllBase = entry + 0x20;
    		PBYTE fullNameBuffer = *(PBYTE*)(entry + 0x38 + 0x8);
    		PBYTE baseNameBuffer = *(PBYTE*)(entry + 0x48 + 0x8);

    		// move to next
    		entry = *(PBYTE*)entry;  // Flink
		}
	*/
    // TODO: remove this when hashing is implemented
    SIZE_T Size = 0;
    //WCHAR ModuleNameW[MAX_PATH] = {0};
    CHAR ModuleName[MAX_PATH] = {0};

    PVOID pModule;
	// NOTE: direct casting will only work for InLoadOrderModuleList because it is the first field of the struct
	PLIST_ENTRY pListHead = &PebAddress->Ldr->InMemoryOrderModuleList;
	PLIST_ENTRY pList = PebAddress->Ldr->InMemoryOrderModuleList.Flink;
	PLDR_DATA_TABLE_ENTRY pDataTableEntry;

	// Find lpModuleName
	while (pList != pListHead)
	{
        pDataTableEntry = CONTAINING_RECORD(pList, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks); // TODO: do CONTAINING_RECORD logic manually 
		
		// will need to do some type conversion
		//if (strcmp(pDataTableEntry->BaseDllName.Buffer, lpModuleName) == 0)
        Size = WCharStringToCharString(ModuleName, pDataTableEntry->BaseDllName.Buffer, pDataTableEntry->BaseDllName.Length);
        //wprintf(L"string = %.*s\n", pDataTableEntry->BaseDllName.Length / sizeof(WCHAR), pDataTableEntry->BaseDllName.Buffer);
        if (Size > 0 && _stricmp(ModuleName, lpModuleName) == 0)
		{
            //printf("found match\n");
			pModule = pDataTableEntry->DllBase;
			break;
		}
		pList = pList->Flink;
	}

	/*
	Offset Arithmetic (64Bit ONLY):
		DWORD e_lfanew = *(DWORD*)(pModule + 0x3c);
		DWORD dwExportDirRVA = *(DWORD*)(pModule + e_lfanew + 0x88);
		PIMAGE_EXPORT_DIRECTORY pExportDir = pModule + dwExportDirRVA;
		// TODO: 
		DWORD NumberOfNames = *(DWORD*)(pExportDir + 0x18);
		DWORD* AddressOfFunctions    = (DWORD*)(pExportDir + 0x1C);
		DWORD* AddressOfNames        = (DWORD*)(pExportDir + 0x20);
		WORD*  AddressOfNameOrdinals = (WORD*)(pExportDir + 0x24);
		
		
	*/
	

	// Find lpProcName
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS) ((ULONG_PTR) pModule + ((PIMAGE_DOS_HEADER) pModule)->e_lfanew); // pModule + 0x3c (32/64bit)
	DWORD dwExportDirRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress; // pModule + 0x3c + 0x88 (64bit)
	PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY) ((ULONG_PTR) pModule + dwExportDirRVA); // pModule + (pModule + 0x3c)

	DWORD* arrayOfFunctionRVAs = (DWORD *)((ULONG_PTR)pModule + pExportDir->AddressOfFunctions); 		// PDWORD
    DWORD* arrayOfNamesRVAs = (DWORD *)((ULONG_PTR)pModule + pExportDir->AddressOfNames);				// PDWORD
    WORD* arrayOfNameOrdinals = (WORD *)((ULONG_PTR)pModule + pExportDir->AddressOfNameOrdinals); 	// PWORD

    /* 
    TODO: attempt to resolve ordinally first...
    check if imports are ordinal, if they are we can immediatly find the address of the 
        function without looping through the whole array via arrayOfFunctionRVAs += ((IMAGE_ORDINAL((DWORD)lpProcName) - pExportDirectory->Base) * sizeof(DWORD));
    */

    for (DWORD i = 0; i < pExportDir->NumberOfNames; ++i)
    {
        char *prodName = (char *)((ULONG_PTR)pModule + arrayOfNamesRVAs[i]);
        WORD ordinalIndex = arrayOfNameOrdinals[i];
        FARPROC functionAddress = (FARPROC)((ULONG_PTR)pModule + arrayOfFunctionRVAs[ordinalIndex]);

        // TODO: idk if case-insensitive strcmp is needed on here
        if (_stricmp(lpProcName, prodName) == 0)
        {
            return functionAddress;
        }
    }
    return NULL;
}
#endif
