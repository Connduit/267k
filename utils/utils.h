#ifndef _UTIL_H
#define _UTIL_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Winsock2.h>
#include <intrin.h>

// TODO: Use offsets to reduce the need to define these LDR structs
// see offsets here: https://struppigel.github.io/WisdomForHedgehogs/Execution%20Environments/PEB%20Walking%20and%20Export%20Parsing/

/*
typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink; // ptr to LDR_DATA_TABLE_ENTRY
   struct _LIST_ENTRY *Blink; // ptr to LDR_DATA_TABLE_ENTRY
} LIST_ENTRY, *PLIST_ENTRY, *RESTRICTED_POINTER PRLIST_ENTRY;
*/

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

// TODO: this struct isn't needed? we only need Ldr anyways
// If it matters: https://github.com/HavocFramework/Havoc/blob/main/payloads/DllLdr/Include/Native.h#L41
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

// GetModule() ? // have 


// first find the lpModuleName (exe/dll name), then find the lpProcName (proceduce/function name)
FARPROC GetProcAddressManual(LPCSTR lpModuleName, LPCSTR lpProcName )
{
    PPEB PebAddress;
    // Get PEB
#if defined(_WIN64)
    PebAddress = (PPEB) __readgsqword( 0x60 );
#else
    PebAddress = (PPEB) __readfsdword( 0x30 );
#endif


	PLIST_ENTRY pList = PebAddress->Ldr->InMemoryOrderModuleList.Flink; // THIS IS A PTR to a LDR_DATA_TABLE_ENTRY
	PLDR_DATA_TABLE_ENTRY pTableEntry;
	//PLDR_DATA_TABLE_ENTRY pTableEntry = CONTAINING_RECORD(pList, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks); // TODO: might have to make my own version of CONTAINING_RECORD
	
	// PLIST_ENTRY pListTail = PebAddress->Ldr->InMemoryOrderModuleList.Blink;
	// PLDR_DATA_TABLE_ENTRY pTableEntry = pList->InMemoryOrderLinks;

	/*
	 PPEB pPeb = GetPEB();
    if (!pPeb || !pPeb->Ldr) return;

    PLIST_ENTRY pListHead = &pPeb->Ldr->InMemoryOrderModuleList;
    PLIST_ENTRY pList = pListHead->Flink;

    while (pList != pListHead)  // loop until we come back to the head
    {
        PLDR_DATA_TABLE_ENTRY pEntry =
            CONTAINING_RECORD(pList, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        // Print module info
        wprintf(L"BaseDllName: %wZ\n", &pEntry->BaseDllName);
        wprintf(L"FullDllName: %wZ\n", &pEntry->FullDllName);
        wprintf(L"DllBase: %p\n\n", pEntry->DllBase);

        // Move to the next node
        pList = pList->Flink;
    }
}
	*/

	//TODO
	PVOID pModuleBase = pTableEntry->DllBase;
	UNICODE_STRING BaseDllName = pTableEntry->BaseDllName;

	// loop through pDataTableEntry until we find match with pDataTableEntry->BaseDllName
	do 
	{
		pTableEntry = CONTAINING_RECORD(pList, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
		pModuleBase = pTableEntry->DllBase;
		if (pTableEntry->BaseDllName == lpModuleName)
		{
			pModule = pTableEntry->DllBase;
			break;
		}
		pList = pList->Flink;
	} 
	while (pList != PebAddress->Ldr->InMemoryOrderModuleList.Flink); // TODO: find out if it is more effient to just save off  PebAddress->Ldr->InMemoryOrderModuleList.Blink
	
	// TODO: 
	PIMAGE_NT_HEADERS  pNtHeaders = (PIMAGE_NT_HEADERS)(pModuleBase + ((PIMAGE_DOS_HEADER)pModuleBase)->e_lfanew);
	PIMAGE_DATA_DIRECTORY pDataDirectory = (PIMAGE_DATA_DIRECTORY)&pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	PIMAGE_EXPORT_DIRECTORY pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(pModuleBase + pDataDirectory->VirtualAddress);




    return NULL;
}
#endif
