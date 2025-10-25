#ifndef CLIENT_H
#define CLIENT_H


#include "Compressor.h"
#include "C2Profile.h"
#include "Encoder.h"
#include "Encryptor.h"
#include "Serializer.h"
//#include "MessagePublisher.h"
//#include "MessageConsumer.h"
#include "MessageHandler.h"
#include "Transporter.h"


#include <memory>


class Client
{//10.0.0.48, 172.18.245.234
public:
    Client() : transporter_(&messageHandler_, "10.0.0.48", 4444) {}
    Client(TCPTransporter transporter, std::string server, uint8_t port) : transporter_(&messageHandler_, server, port) {}

    //Client(Encryptor& encryptor, Encoder& encoder, Serializer& serializer, C2Profile& config);
	//bool run(const char* host, const char* port);// TODO: change to private... when done debugging
    bool run();
private:

    //Transporter transporter_;
    TCPTransporter transporter_; // NOTE: hardcode transport type?
    MessageHandler messageHandler_;

    //Compressor& compressor_;
    //Encryptor& encryptor_;
    //Encoder& encoder_;
    //Serializer& serializer_;
	//MessageConsumer messageConsumer_;
	//MessagePublisher messagePublisher_;

    //MessageConsumer messageConsumer_;
    //MessagePublisher messagePublisher_;
	//C2ProfileUniquePtr configPtr_;
	//
    //MessageHandler messageHandler_; 
    //C2Profile& config_; // TODO: eventually have 

};


#endif // CLIENT_H


