#include "Transporter.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#pragma comment(lib, "ws2_32.lib")

inline bool Transporter::sendMessage(const InternalMessage& msg)
{
    auto serialized = serializer_.serialize(msg);
    auto encoded = encoder_.encode(serialized);
    auto encrypted = encryptor_.encrypt(encoded);
    return send(encrypted);
}

InternalMessage Transporter::receiveMessage()
{
    auto data = receive();
    if (data.empty()) return InternalMessage();

    auto decrypted = encryptor_.decrypt(data);
    auto decoded = encoder_.decode(decrypted);
    return serializer_.deserialize(decoded);
}

void Transporter::beacon()
{
    auto heartbeat = createHeartbeat();
    sendMessage(heartbeat);

    auto incoming = receiveMessage();
    if (incoming.header.messageType != DEFAULT)
    {
        handleIncomingMessage(incoming);
    }
}

InternalMessage Transporter::createHeartbeat()
{
    InternalMessage msg;
    msg.header.messageType = MessageType::HEARTBEAT;
    msg.header.messageId = generateId();
    msg.header.dataSize = 0;
    return msg;
}

void Transporter::handleIncomingMessage(const InternalMessage& msg)
{
    // TODO; 
}

uint32_t Transporter::generateId()
{
    return rand();
}




TCPTransporter::TCPTransporter(MessageHandler* hdlr, const std::string& server, uint16_t port)
//TCPTransporter::TCPTransporter(MessageHandler* hdlr, const std::string& server, std::string port)
    : Transporter(hdlr), server_(server), port_(port)
{
    initializeWinsock();
}

bool TCPTransporter::initializeWinsock()
{
    // TODO: check if winsock is already initialized somehow?
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "WSAStartup failed" << std::endl;
        // printf("WSAStartup failed\n");
        return true;
    }
    return false;
    //winsock_initialized = true;
    
}

bool TCPTransporter::send(const std::vector<uint8_t>& data)
{
    if (!connected_ && !connect()) return false;
    
    return ::send(socket_, (const char*)data.data(), data.size(), 0) > 0;

}

bool TCPTransporter::connect()
{
    std::cout << "inside connect" << std::endl;
    socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET)
    {
        std::cout << "socket invalid" << std::endl;
        return false;
    }
    


    ADDRINFOA hints, *result = nullptr;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    //if (getaddrinfo(server_.c_str(), (std::to_string(port_)).c_str(), &hints, &result) != 0)
    if (getaddrinfo(server_.c_str(), "4444", &hints, &result) != 0)
    {
        //std::cout << "getaddrinfo failed" << std::endl;
        printf("getaddrinfo failed\n");
        return false;
    }
    /*
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    inet_pton(AF_INET, server_.c_str(), &addr.sin_addr);*/

    ADDRINFOA* ptr = result; // TODO: no need to assign result to ptr just use result?
    while (ptr != NULL)
    {
        if (::connect(socket_, ptr->ai_addr, (int)ptr->ai_addrlen) == 0)
        {
            std::cout << "connected == true" << std::endl;
            connected_ = true;
            break;
        }
        ptr = ptr->ai_next;
    }

    if (connected_ == false)
    {
        std::cout << "connected_ == false" << std::endl;
    }

    /*
    if (::connect(socket_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cout << "failed to connect" << std::endl;
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }*/

    return true;
}

std::vector<uint8_t> TCPTransporter::receive()
{
    if (!connected_) return {};

    std::vector<uint8_t> buffer(4096);
    int received = recv(socket_, (char*)buffer.data(), buffer.size(), 0);
    if (received <= 0)
    {
        connected_ = false;
        return {};
    }
    buffer.resize(received);
    return buffer;
}


