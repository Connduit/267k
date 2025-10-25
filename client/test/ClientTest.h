#ifndef CLIENT_TEST_H
#define CLIENT_TEST_H

#include "MessageTypes.h"
#include "Serializer.h"

class ClientTest
{
public:
	bool testSerializer(InternalMessage& msg, BinarySerializer& serializer);
	bool testAll();
private:
	// Client class object
};

#endif