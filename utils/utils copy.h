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
	PPEB_LDR_DATA ldr = PebAddress->Ldr;
	PLIST_ENTRY list = ldr->InMemoryOrderModuleList.Flink;

	while (list != &ldr->InMemoryOrderModuleList)
	{
		PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(list, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

		if (entry->BaseDllName.Buffer)
		{
			// Simple check for "kernel32.dll"
			if (entry->BaseDllName.Length >= 24 && // "kernel32.dll" is 12 chars * 2 bytes
				entry->BaseDllName.Buffer[0] == L'k' &&
				entry->BaseDllName.Buffer[6] == L'3' &&
				entry->BaseDllName.Buffer[7] == L'2')
			{
				return (HMODULE)entry->DllBase;
			}
		}
		list = list->Flink;
	}
	//return NULL;

	PBYTE base = (PBYTE)module;
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(base + dos->e_lfanew);

	DWORD export_rva = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)(base + export_rva);

	DWORD *functions = (DWORD *)(base + exports->AddressOfFunctions);
	DWORD *names = (DWORD *)(base + exports->AddressOfNames);
	WORD *ordinals = (WORD *)(base + exports->AddressOfNameOrdinals);

	for (DWORD i = 0; i < exports->NumberOfNames; i++)
	{
		char *name = (char *)(base + names[i]);
		if (strcmp(name, func_name) == 0)
		{
			return (FARPROC)(base + functions[ordinals[i]]);
		}
	}
	return NULL;
}
/*
// ReactOs: https://github.com/reactos/reactos/blob/master/dll/win32/kernel32/client/loader.c
FARPROC
	WINAPI
GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	ANSI_STRING ProcedureName, *ProcNamePtr = NULL;
	FARPROC fnExp = NULL;
	NTSTATUS Status;
	PVOID hMapped;
	ULONG Ordinal = 0;

	if ((ULONG_PTR)lpProcName > MAXUSHORT)
	{
		// Look up by name 
		RtlInitAnsiString(&ProcedureName, (LPSTR)lpProcName);
		ProcNamePtr = &ProcedureName;

	}
	else
	{
		// Look up by ordinal 
		Ordinal = PtrToUlong(lpProcName);

	}

	// Map provided handle 
	hMapped = BasepMapModuleHandle(hModule, FALSE);

	// Get the proc address
	// LdrGetProcedureAddress is an undocumented function of NTDLL
	// def shouldn't use it as it is probs hooked by av. also it's a lower level version of GetProcAddress and rarely used by legitimate software 
	Status = LdrGetProcedureAddress(hMapped, 
			ProcNamePtr,
			Ordinal,
			(PVOID*)&fnExp);

	if (!NT_SUCCESS(Status))
	{
		// BaseSetLastNTError is an undocumented Windows function... probs shouldn't use either
		// manually doing everything is less suspicous than calling undocumented funcs
		BaseSetLastNTError(Status);
		return NULL;

	}

	// Check for a special case when returned pointer is the same as image's base address 
	if (fnExp == hMapped)
	{
		// Set correct error code 
		if (HIWORD(lpProcName) != 0)
			BaseSetLastNTError(STATUS_ENTRYPOINT_NOT_FOUND);
		else
			BaseSetLastNTError(STATUS_ORDINAL_NOT_FOUND);

		return NULL;

	}

	// All good, return procedure pointer 
	return fnExp;

}

// TODO: https://github.com/arbiter34/GetProcAddress/blob/master/GetProcAddress/GetProcAddress.cpp
// GetProcAddress
// Use: Independent GetProcAddress using TIB/PEB/LDR
// Parameters: wchar_t string with DLL Name, wchar_t string with Function Name
// Return: void* to Function - nullptr if not found

void *GetProcAddress(wchar_t* dllName, wchar_t* procName) {
	void *procAddr = nullptr;

	//Get Table Entry for DLL
	PLDR_DATA_TABLE_ENTRY dllEntry = GetLdrDataTableEntry(dllName);

	if (dllEntry == nullptr) {
		return nullptr;
	}

	//DllBase as unsigned long for arithmetic
	unsigned long long dllBaseAddr = (unsigned long long)dllEntry->DllBase;

	//Cast DllBase to use struct
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)dllBaseAddr; 
	
	//Calculate NTHeader and Cast
	PIMAGE_NT_HEADERS64 pNtHeader = (PIMAGE_NT_HEADERS64)(dllBaseAddr + dosHeader->lfanew);

	//Calculate ExportDir Address and Cast
	PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)(dllBaseAddr + pNtHeader->OptionalHeader.DataDirectory[0].VirtualAddress);

	//Calculate AddressOfNames Absolute and Cast
	unsigned int *NameRVA = (unsigned int*)(dllBaseAddr + pExportDir->AddressOfNames);

	//Iterate over AddressOfNames
	for (int i = 0; i < pExportDir->NumberOfNames; i++) {
		//Calculate Absolute Address and cast
		char* name = (char*)(dllBaseAddr + NameRVA[i]);
		wchar_t *wname = CharToWChar_T(name);
		if (StringMatches(wname, procName)) {
			free(wname);

			//Lookup Ordinal
			unsigned short NameOrdinal = ((unsigned short*)(dllBaseAddr + pExportDir->AddressOfNameOrdinals))[i];

			//Use Ordinal to Lookup Function Address and Calculate Absolute
			unsigned int addr = ((unsigned int*)(dllBaseAddr + pExportDir->AddressOfFunctions))[NameOrdinal];
			
			//Function is forwarded
			if (addr > pNtHeader->OptionalHeader.DataDirectory[0].VirtualAddress && addr < pNtHeader->OptionalHeader.DataDirectory[0].VirtualAddress + pNtHeader->OptionalHeader.DataDirectory[0].Size) {
				//Grab and Parse Forward String
				char* forwardStr = (char*)(dllBaseAddr + addr);

				wchar_t** str_arr = ParseForwardString(forwardStr);

				//Attempt to load library if not loaded
				if (!LibraryLoaded(str_arr[0])) {
					void* dllBase = _LoadLibraryW(str_arr[0]);
				}

				//Recurse using forward information
				procAddr = GetProcAddress(str_arr[0], str_arr[1]);
				free(str_arr[0]);
				free(str_arr[1]);
				free(str_arr);
			}
			else {
				procAddr = (void*)(dllBaseAddr + addr);
			}
			break;
		}
		if (wname != nullptr) {
			free(wname);
		}
	}
	return procAddr;
}

// random dudes online forum
FARPROC get_proc_addr(HMODULE module, const char* func_name) {
	PBYTE base = (PBYTE)module;
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(base + dos->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)(
			base + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress
			);

	PDWORD functions = (PDWORD)(base + exports->AddressOfFunctions);
	PDWORD names = (PDWORD)(base + exports->AddressOfNames);
	PWORD ordinals = (PWORD)(base + exports->AddressOfNameOrdinals);

	for (DWORD i = 0; i < exports->NumberOfNames; i++) {
		char* name = (char*)(base + names[i]);
		if (strcmp(name, func_name) == 0) {
			return (FARPROC)(base + functions[ordinals[i]]);

		}

	}
	return NULL;

}


// deepseek: GetProcessAddress
FARPROC get_proc_addr(HMODULE module, const char* func_name) {
    PBYTE base = (PBYTE)module;
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return NULL;
    
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return NULL;
    
    DWORD export_rva = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (!export_rva) return NULL;
    
    PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)(base + export_rva);
    
    DWORD* functions = (DWORD*)(base + exports->AddressOfFunctions);
    DWORD* names = (DWORD*)(base + exports->AddressOfNames);
    WORD* ordinals = (WORD*)(base + exports->AddressOfNameOrdinals);
    
    for (DWORD i = 0; i < exports->NumberOfNames; i++) {
        char* name = (char*)(base + names[i]);
        if (strcmp(name, func_name) == 0) {
            return (FARPROC)(base + functions[ordinals[i]]);
        }
    }
    return NULL;
}


// metasploit
HMODULE GetProcAddressWithHash( _In_ DWORD dwModuleFunctionHash )
{
	PPEB PebAddress;
	PMY_PEB_LDR_DATA pLdr;
	PMY_LDR_DATA_TABLE_ENTRY pDataTableEntry;
	PVOID pModuleBase;
	PIMAGE_NT_HEADERS pNTHeader;
	DWORD dwExportDirRVA;
	PIMAGE_EXPORT_DIRECTORY pExportDir;
	PLIST_ENTRY pNextModule;
	DWORD dwNumFunctions;
	USHORT usOrdinalTableIndex;
	PDWORD pdwFunctionNameBase;
	PCSTR pFunctionName;
	UNICODE_STRING BaseDllName;
	DWORD dwModuleHash;
	DWORD dwFunctionHash;
	PCSTR pTempChar;
	DWORD i;

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
		dwModuleHash = 0;
		pModuleBase = pDataTableEntry->DllBase;
		BaseDllName = pDataTableEntry->BaseDllName;
		pNTHeader = (PIMAGE_NT_HEADERS) ((ULONG_PTR) pModuleBase + ((PIMAGE_DOS_HEADER) pModuleBase)->e_lfanew);
		dwExportDirRVA = pNTHeader->OptionalHeader.DataDirectory[0].VirtualAddress;

		// Get the next loaded module entry
		pDataTableEntry = (PMY_LDR_DATA_TABLE_ENTRY) pDataTableEntry->InLoadOrderLinks.Flink;

		// If the current module does not export any functions, move on to the next module.
		if (dwExportDirRVA == 0)
		{
			continue;
		}

		// Calculate the module hash
		for (i = 0; i < BaseDllName.MaximumLength; i++)
		{
			pTempChar = ((PCSTR) BaseDllName.Buffer + i);

			dwModuleHash = ROTR32( dwModuleHash, 13 );

			if ( *pTempChar >= 0x61 )
			{
				dwModuleHash += *pTempChar - 0x20;
			}
			else
			{
				dwModuleHash += *pTempChar;
			}
		}

		pExportDir = (PIMAGE_EXPORT_DIRECTORY) ((ULONG_PTR) pModuleBase + dwExportDirRVA);

		dwNumFunctions = pExportDir->NumberOfNames;
		pdwFunctionNameBase = (PDWORD) ((PCHAR) pModuleBase + pExportDir->AddressOfNames);

		for (i = 0; i < dwNumFunctions; i++)
		{
			dwFunctionHash = 0;
			pFunctionName = (PCSTR) (*pdwFunctionNameBase + (ULONG_PTR) pModuleBase);
			pdwFunctionNameBase++;

			pTempChar = pFunctionName;

			do
			{
				dwFunctionHash = ROTR32( dwFunctionHash, 13 );
				dwFunctionHash += *pTempChar;
				pTempChar++;
			} while (*(pTempChar - 1) != 0);

			dwFunctionHash += dwModuleHash;

			if (dwFunctionHash == dwModuleFunctionHash)
			{
				usOrdinalTableIndex = *(PUSHORT)(((ULONG_PTR) pModuleBase + pExportDir->AddressOfNameOrdinals) + (2 * i));
				return (HMODULE) ((ULONG_PTR) pModuleBase + *(PDWORD)(((ULONG_PTR) pModuleBase + pExportDir->AddressOfFunctions) + (4 * usOrdinalTableIndex)));
			}
		}
	}

	// All modules have been exhausted and the function was not found.
	return NULL;
}
*/