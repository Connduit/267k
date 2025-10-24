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
	//Client() {};
    //Client(C2ProfileUniquePtr config); // TODO: eventually config should be a uniquePtr?
    //Client(Compressor& compressor, Encryptor& encryptor, Encoder& encoder, Serializer& serializer, C2Profile& config);
    Client(Encryptor& encryptor, Encoder& encoder, Serializer& serializer, C2Profile& config);
	bool run(const char* host, const char* port);// TODO: change to private... when done debugging
private:
    //CompressorUniquePtr compressorPtr_;
    //EncryptorUniquePtr encryptorPtr_;
    //EncoderUniquePtr encoderPtr_;
    //SerializerUniquePtr serializerPtr_;
	//MessageConsumerPtr messageConsumerPtr_;
	//MessagePublisherPtr messagePublisherPtr_;

    //Compressor& compressor_;
    Encryptor& encryptor_;
    Encoder& encoder_;
    Serializer& serializer_;
	//MessageConsumer messageConsumer_;
	//MessagePublisher messagePublisher_;

    //MessageConsumer messageConsumer_;
    //MessagePublisher messagePublisher_;
	//C2ProfileUniquePtr configPtr_;
	//
    //MessageHandler messageHandler_; 
    C2Profile& config_; // TODO: eventually have 

};

#endif
