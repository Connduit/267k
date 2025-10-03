#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <winsock2.h>

#include <vector>
#include <ws2tcpip.h>


#include <iostream>
//#include <WS2tcpip.h>
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
    //std::string receiveData();
	std::vector<unsigned char> receiveData();
    void execute(std::string program="notepad.exe");
    void sendData(const std::string& data);
    void parse();
    void getSystemInfo();
    // TODO: add setsockopt handling function to enable socket options like SO_KEEPALIVE
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

#endif // CLIENT_H
