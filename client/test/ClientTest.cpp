#include "test.h" // TODO: not getting the includes from here for some reason?
#include "ClientTest.h"
#include "Client.h"


#include <cassert>
#include <iostream>

bool ClientTest::testAll()
{
	XorEncryptor encryptor;
	B64Encoder encoder;
	BinarySerializer serializer;
	C2Profile config;

	//Client client(compressor, encryptor, encoder, serializer, config);
	Client client(encryptor, encoder, serializer, config);

	InternalMessage msg;
	MessageHeader header;
	header.messageType = MessageType::SYS_INFO;
	header.dataSize = 32;
	header.messageId = 99;
	msg.data = internalMessageData;

	testSerializer(msg, serializer);

	return false;
}

bool ClientTest::testSerializer(InternalMessage& msg, BinarySerializer& serializer)
{
	std::vector<uint8_t> outMsg;
	serializer.serialize(msg, outMsg);
	for (auto b : outMsg)
	{
		std::cout << static_cast<int>(b) << " ";
	}

	return false;
}