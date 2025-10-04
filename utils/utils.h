#include <windows.h>

FARPROC GetProcAddressManual(HMODULE hModule, LPCSTR lpProcName)
{

#if defined(_WIN64)
	PebAddress = (PPEB) __readgsqword(0x60);
#else
	PebAddress = (PPEB) __readfsdword(0x30);
#endif


}

// ReactOs: https://github.com/reactos/reactos/blob/master/dll/win32/kernel32/client/loader.c
/*
 *  * @implemented
 *   */
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
		/* Look up by name */
		RtlInitAnsiString(&ProcedureName, (LPSTR)lpProcName);
		ProcNamePtr = &ProcedureName;

	}
	else
	{
		/* Look up by ordinal */
		Ordinal = PtrToUlong(lpProcName);

	}

	/* Map provided handle */
	hMapped = BasepMapModuleHandle(hModule, FALSE);

	/* Get the proc address */
	Status = LdrGetProcedureAddress(hMapped,
			ProcNamePtr,
			Ordinal,
			(PVOID*)&fnExp);

	if (!NT_SUCCESS(Status))
	{
		BaseSetLastNTError(Status);
		return NULL;

	}

	/* Check for a special case when returned pointer is
	 *        the same as image's base address */
	if (fnExp == hMapped)
	{
		/* Set correct error code */
		if (HIWORD(lpProcName) != 0)
			BaseSetLastNTError(STATUS_ENTRYPOINT_NOT_FOUND);
		else
			BaseSetLastNTError(STATUS_ORDINAL_NOT_FOUND);

		return NULL;

	}

	/* All good, return procedure pointer */
	return fnExp;

}

// TODO: https://github.com/arbiter34/GetProcAddress/blob/master/GetProcAddress/GetProcAddress.cpp


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
