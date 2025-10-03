#include <windows.h>

FARPROC GetProcAddressManual(HMODULE hModule, LPCSTR lpProcName)
{

#if defined(_WIN64)
	PebAddress = (PPEB) __readgsqword(0x60);
#else
	PebAddress = (PPEB) __readfsdword(0x30);
#endif


}

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
