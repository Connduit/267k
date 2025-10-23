#ifndef CLIENT_H
#define CLIENT_H


#include "Compressor.h"
#include "C2Profile.h"
#include "Encoder.h"
#include "Encryptor.h"
#include "Serializer.h"
#include "MessagePublisher.h"
#include "MessageConsumer.h"

#include <memory>


class Client
{
public:
    Client(C2Profile config); // TODO: eventually config should be a uniquePtr?
private:
    CompressorUniquePtr compressorPtr_;
    EncryptorUniquePtr encryptorPtr_;
    EncoderUniquePtr encoderPtr_;
    SerializerUniquePtr serializerPtr_;
    MessageConsumer messageConsumer_;
    MessagePublisher messagePublisher_;
    MessageHandler messageHandler_; 
    // C2ProfileUniquePtr configPtr_; // TODO: eventually have 

	bool run(const char* host, const char* port);
};

#endif
