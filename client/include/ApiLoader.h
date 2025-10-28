#ifndef API_LOADER_H
#define API_LOADER_H


// TODO: maybe rename to api loader? or libLoaderApi?
class ApiLoader
{
public:
    //ApiLoader();
private:
	//PebWalker (OWNS PEB traversal logic)
	//HashManager (OWNS API hashing algorithms)... hashes should be predefined?
	//std::unordered_map<uint32_t, FARPROC> (OWNS API address cache)... maps api to resolved version?
	//
	// 1. RESOLVE phase (finding)
	FARPROC resolveAddress(const string& function_name);
	// Walk PEB, find module, parse exports, get address

	// 2. LOAD phase (preparing)  
	template<typename FuncType>
	FuncType loadFunction(const string& function_name) 
	{
		FARPROC address = resolveAddress(function_name);
		return reinterpret_cast<FuncType>(address);  // Ready to call!
	}

	// HMODULE GetModuleHandleManualHashO(DWORD moduleHash); // using offsets
	// GetModuleHandle by literal name
	HMODULE GetModuleHandleManual(LPCWSTR lpModuleName);
	// GetModuleHandle by hash
	HMODULE GetModuleHandleManual(DWORD moduleHash);
	
	// FARPROC GetProcAddressManual(LPCSTR lpModuleName, LPCSTR lpProcName); remove version? it does GetModuleHandle inside of it
	// GetProcAddress by literal name
	FARPROC GetProcAddressManual(PVOID pModule, LPCSTR lpProcName);
	// GetProcAddress by hash
	FARPROC GetProcAddressManual(HMODULE hModule, DWORD procHash);

	// TODO: move these outside of class?
	CHAR HideChar(CHAR c) { return c; }
	WCHAR HideChar(WCHAR c) { return c; }




};

// TODO: define hashed apis here?

#endif
