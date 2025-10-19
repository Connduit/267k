/* MessageConsumer.h (rename later?) */
/* MessageConsumer.h handles all incoming messages from server */

/* define structures of incoming messages/packets */

// TODO: data validition

// TODO: all communication layers should be established here?
// 		 tcp, http/s, dns, ssl/tls

#include "cryptography/decrypt.h"
#include "encoders/decode.h"

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


int handleTCP(uint8_t* data)
{
	// Raw Bytes from Socket
	uint8_t buf[4096];

	// DECRYPT -> Decrypted Bytes
	decrypt_aes_256_gcm(data, length(data), key, &buf, sizeof() ;

	// DESERIALIZE -> InternalMessage 
	

	// VALIDATE

	// EXECUTE (InternalMessage)
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


