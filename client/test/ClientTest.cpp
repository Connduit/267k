#include "test.h" // TODO: not getting the includes from here for some reason?
#include "ClientTest.h"
#include "Client.h"


#include <cassert>
#include <iostream>

bool ClientTest::testAll()
{
	XorEncryptor encryptor("testKey");
	B64Encoder encoder;
	BinarySerializer serializer;
	C2Profile config;

	//Client client(compressor, encryptor, encoder, serializer, config);
	Client client(encryptor, encoder, serializer, config);

	// InternalMessage
	InternalMessage inMsg;
	MessageHeader header;
	header.messageType = MessageType::SYS_INFO;
	header.dataSize = 32;
	header.messageId = 99;
	inMsg.data = internalMessageData;

	// TODO: Validate?

	// Serialize
	std::vector<uint8_t> serializerOutMsg;
	testSerializer(inMsg, serializerOutMsg, serializer);

	// Encode
	//std::vector<uint8_t> encoderOutMsg;
	std::string encoderOutMsg;
	testEncoder(serializerOutMsg, encoderOutMsg, encoder);

	// TODO: Compress? 

	//std::vector<uint8_t> encryptOutMsg;
	// TODO: Encrypt
	//testEncrypter(encoderOutMsg, encryptOutMsg, encryptor);

	// TODO: Frame (if needed)

	// TODO: Raw Bytes

	// TODO: Send

	return false;
}

bool ClientTest::testSerializer(InternalMessage& inMsg, std::vector<uint8_t>& outMsg,BinarySerializer& serializer)
{
	serializer.serialize(inMsg, outMsg);
	for (auto b : outMsg)
	{
		std::cout << static_cast<int>(b) << " ";
	}
	std::cout << std::endl;

	return false;
}

//bool ClientTest::testEncoder(std::string& inMsg, std::vector<uint8_t>& outMsg, B64Encoder& encoder)
bool ClientTest::testEncoder(std::vector<uint8_t>& inMsg, std::string& outMsg , B64Encoder& encoder)
{
	std::cout << "inside testEncoder" << std::endl;
	encoder.encode(inMsg, outMsg);
	for (auto b : outMsg)
	{
		std::cout << static_cast<int>(b) << " ";
	}
	std::cout << std::endl;
	return false;
}

bool ClientTest::testEncrypter(std::vector<uint8_t>& inMsg, std::vector<uint8_t>& outMsg, XorEncryptor& encryptor)
{
	encryptor.encrypt(inMsg, outMsg);
	for (auto b : outMsg)
	{
		std::cout << static_cast<int>(b) << " ";
	}
	std::cout << std::endl;
	return false;

}
