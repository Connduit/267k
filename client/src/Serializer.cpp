/* Serializer.cpp */

#include "Serializer.h"

bool BinarySerializer::serialize(const InternalMessage& msg, std::vector<uint8_t>& outMsg)
{
	outMsg.clear();
	outMsg.reserve(sizeof(msg.header) + msg.data.size());


	const uint8_t* headerBytes = reinterpret_cast<const uint8_t*>(&msg.header);
	outMsg.insert(outMsg.end(), headerBytes, headerBytes + sizeof(msg.header));

	outMsg.insert(outMsg.end(), msg.data.begin(), msg.data.end());

	return false;
}


/*


int serializeReconMessage(const ReconMessage* msg, BYTE* buffer) {
	BYTE* ptr = buffer; // uint8_t*

	// Copy each field from msg struct to buffer
	memcpy(ptr, &msg->dwMajorVersion, sizeof(ULONG)); ptr += sizeof(ULONG);
	memcpy(ptr, &msg->dwMinorVersion, sizeof(ULONG)); ptr += sizeof(ULONG);
	memcpy(ptr, &msg->dwBuildNumber, sizeof(ULONG)); ptr += sizeof(ULONG);
	memcpy(ptr, &msg->wProductType, sizeof(UCHAR)); ptr += sizeof(UCHAR);
	// ... other fields
	return ptr - buffer; // Returns total bytes copied
}

// int deserialize() {}
*/
