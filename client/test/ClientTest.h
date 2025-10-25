#ifndef CLIENT_TEST_H
#define CLIENT_TEST_H

#include "C2Profile.h"
#include "Compressor.h"
#include "Encoder.h"
#include "Encryptor.h"
#include "Serializer.h"

#include "MessageTypes.h"

class ClientTest
{
public:
	bool testSerializer(InternalMessage& inMsg, std::vector<uint8_t>& outMsg, BinarySerializer& serializer);
	bool testEncrypter(std::vector<uint8_t>& inMsg, std::vector<uint8_t>& outMsg, XorEncryptor& encryptor);
	bool testEncoder(std::vector<uint8_t>& inMsg, std::string& outMsg , B64Encoder& encoder);
	bool testAll();

	// void printRawData(std::vector<uint8_t>& msg);
private:
	// Client class object
};

#endif
