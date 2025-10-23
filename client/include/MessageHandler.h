/* MessageHandler.h */
#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "C2Profile.h"
#include <vector>


class MessageHandler
{
public:
	//MessageHandler(C2Profile& config) {}; // TODO:
	MessageHandler();
    
    bool sendMessage(); // overload this function?
    bool recvMessage(); // overload this function?
	//bool receiveMessages(uint8_t* buffer, size_t bytes_received, C2Profile& config);
	bool receiveMessages(uint8_t* buffer, size_t bytes_received);
private:
	//C2Profile& config_;
	// Connections connection_ // MessageHandler should own/control socket/connections?

};


typedef std::unique_ptr<MessageHandler> MessageHandlerPtr;

#endif
