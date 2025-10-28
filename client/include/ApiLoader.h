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

	HMODULE GetModuleHandleManualHash(DWORD moduleHash);
	FARPROC GetProcAddressManualM(PVOID pModule, LPCSTR lpProcName);
	FARPROC GetProcAddressManual(LPCSTR lpModuleName, LPCSTR lpProcName);
	FARPROC GetProcAddressManualHash(HMODULE hModule, DWORD procHash);



};

// TODO: define hashed apis here?

#endif
