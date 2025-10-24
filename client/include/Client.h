#ifndef CLIENT_H
#define CLIENT_H


#include "Compressor.h"
#include "C2Profile.h"
#include "Encoder.h"
#include "Encryptor.h"
#include "Serializer.h"
#include "MessagePublisher.h"
#include "MessageConsumer.h"
#include "MessageHandler.h"

#include <memory>


class Client
{
public:
	Client() {};
    Client(C2ProfileUniquePtr config); // TODO: eventually config should be a uniquePtr?
    //Client(C2Profile config); // TODO: eventually config should be a uniquePtr?
	bool run(const char* host, const char* port);// TODO: change to private... when done debugging
private:
    CompressorUniquePtr compressorPtr_;
    EncryptorUniquePtr encryptorPtr_;
    EncoderUniquePtr encoderPtr_;
    SerializerUniquePtr serializerPtr_;
	MessageConsumerPtr messageConsumerPtr_;
	MessagePublisherPtr messagePublisherPtr_;

    //MessageConsumer messageConsumer_;
    //MessagePublisher messagePublisher_;
    MessageHandler messageHandlerPtr_; 
    // C2ProfileUniquePtr configPtr_; // TODO: eventually have 
	C2ProfileUniquePtr config_;

};

#endif
