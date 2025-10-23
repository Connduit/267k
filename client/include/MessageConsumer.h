/* MessageConsumer.h handles all incoming messages from server */
#ifndef MESSAGE_CONSUMER_H
#define MESSAGE_CONSUMER_H


#include "C2Profile.h"
#include "Encoder.h"
#include "Encryptor.h"
#include "Serializer.h"
#include "MessageHandler.h"



class MessageConsumer
{
public:
	MessageConsumer(
		Encryptor& encryptor,
		Encoder& encoder,
		Serializer& serializer);
	
	bool recvMessage(std::vector<uint8_t> msg);
private:
	//bool recvMessage(uint8_t* msg);
	//MessageConsumer(const MessageConsumer&) = delete;
	//MessageConsumer& operator=(const MessageConsumer&) = delete;

	Encryptor& encryptor_;
	Encoder& encoder_;
	Serializer& serializer_;
};
typedef std::unique_ptr<MessageConsumer> MessageConsumerPtr;
#endif
