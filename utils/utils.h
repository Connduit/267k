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

// TODO: this struct isn't needed? we only need Ldr anyways... might have to uncomment to avoid using certain #includes tho
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


// HMODULE GetModuleHandleManual(LPCSTR lpModuleName) ? 

// first find the lpModuleName (exe/dll name), then find the lpProcName (proceduce/function name)
FARPROC GetProcAddressManual(LPCSTR lpModuleName, LPCSTR lpProcName)
{
    PPEB PebAddress;
    // Get PEB
#if defined(_WIN64)
    PebAddress = (PPEB) __readgsqword( 0x60 );
#else
    PebAddress = (PPEB) __readfsdword( 0x30 );
#endif
	/*
	Offset Arithmetic (64Bit ONLY):
		PEB_LDR_DATA ldr = (BYTE *)PebAddress + 0x18;
		PLIST_ENTRY pListHead = PebAddress + 0x18 + 0x20
		PBYTE moduleList = ldr + 0x20;
		PebAddress                   gs:0x60
+ 0x18 → Ldr (_PEB_LDR_DATA)
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
		if (pDataTableEntry->BaseDllName.Buffer == lpModuleName) // TODO: fix
		{
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

	// TODO: type cast these for clarity? 
	DWORD* arrayOfFunctionRVAs = pModule + pExportDir->AddressOfFunctions; 		// PDWORD
    DWORD* arrayOfNamesRVAs = pModule + pExportDir->AddressOfNames;				// PDWORD
    WORD* arrayOfNameOrdinals = pModule + pExportDir->AddressOfNameOrdinals; 	// PWORD

	for (DWORD i = 0; i < pExportDir->NumberOfNames; ++i)
	{
		char* prodName = (char *)(pModule + arrayOfNamesRVAs[i]);
		WORD ordinalIndex = arrayOfNameOrdinals[i];
		FARPROC functionAddress = pModule + arrayOfFunctionRVAs[ordinalIndex];

		if (strcmp(lpProcName, prodName) == 0)
		{
			return functionAddress;
		}
	}
    return NULL;
}
#endif
