#ifndef TRANSPORTER_H
#define TRANSPORTER_H
/*
├── Transporter (OWNS connection + processing)
│   ├── ConnectionManager (OWNS socket/network)
│   ├── BinarySerializer
│   ├── Base64Encoder
│   └── XOREncryptor
*/

#include <vector>
#include <memory>
#include <string>
#include <WinSock2.h>
//#include <ws2tcpip.h>

class Transporter
{
public:
	// constructor
	// deconstructor
	//virtual ~Transporter() = default; // TODO: what does default do?
	//virtual ~Transporter() = default;
	//virtual bool connect() = 0;
	//virtual bool send(const std::vector<uint8_t>& data) = 0;
	//virtual std::vector<uint8_t> receive() = 0;
	//virtual bool isConnected() = 0;
private:
	/*
	std::string server;
	uint16_t port;
	SOCKET socket = INVALID_SOCKET;
	bool connected = false;
	*/
};



class TCPTransporter : public Transporter
{
public:
	TCPTransporter(const std::string& server, uint16_t port) : server_(server), port_(port) {}
	bool send(const std::vector<uint8_t>& data);
	bool connect();
	std::vector<uint8_t> receive();
	bool isConnected() { return connected_; }
private:
	std::string server_;
	uint16_t port_;
	SOCKET socket_ = INVALID_SOCKET;
	bool connected_ = false;
};

// Handles HTTPS headers, TLS, cookies, etc.
class HTTPSTransport : public Transporter
{

public:
private:
};

// Handles DNS queries/responses, tunneling
class DNSTransport : public Transporter
{

public:
private:
};

typedef std::unique_ptr<Transporter> TransporterUniquePtr;

#endif
