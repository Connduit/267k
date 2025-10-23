#include "MessageConsumer.h"



MessageConsumer::MessageConsumer(
		Encryptor& encryptor,
		Encoder& encoder,
		Serializer& serializer)
		:
		encryptor_(encryptor),
		encoder_(encoder),
		serializer_(serializer)
{}

	//bool recvMessage(uint8_t* msg);
	//Encryptor& encryptor_;
	//Encoder& encoder_;
	//Serializer& serializer_;




bool MessageConsumer::recvMessage(std::vector<uint8_t> msg)
{}
