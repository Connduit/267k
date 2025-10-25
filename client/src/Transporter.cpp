#include "Transporter.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


bool TCPTransporter::send(const std::vector<uint8_t>& data)
{
    if (!connected_ && !connect()) return false;
    
    return ::send(socket_, (const char*)data.data(), data.size(), 0) > 0;

}

bool TCPTransporter::connect()
{
    socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    inet_pton(AF_INET, server_.c_str(), &addr.sin_addr);

    if (::connect(socket_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }

    connected_ = true;
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