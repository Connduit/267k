/* TODO:
 * x86_64-w64-mingw32-gcc -nostdlib -nostartfiles -s -Wl,--entry=main -Wl,--strip-all client.c -o client.exe
 * 
 * // Working flags below, 5/72
 * x86_64-w64-mingw32-gcc -s -Wl,--entry=main -Wl,--strip-all client.c -o client.exe 
 * */

// TODO: eventually convert to c code then asm then shell code?

//#include "stager.h"


#include "MessageConsumer.h"
#include "Client.h"

#include "../common/logging.h"




/*
//Client::Client(C2Profile config) : 
Client::Client(C2ProfileUniquePtr config) : 
	encryptorPtr_(std::make_unique<Encryptor>()), // TODO: have to choose child encryptor
	encoderPtr_(std::make_unique<Encoder>()), // TODO: have to choose child Encoder
	serializerPtr_(std::make_unique<Serializer>()), // TODO: have to choose child Serializer
	messageConsumerPtr_(nullptr), // TODO: should have to be a usingPtr
	messagePublisherPtr_(nullptr)//, // TODO: should have to be a usingPtr
	//messageHandlerPtr_(nullptr) // TODO: should have to be a usingPtr

	//messageHandler_(nullptr)
	//configPtr_(std::make_unique<C2Profile>()),
	//config_(config),
{
	//TODO:
	//encryptorPtr_->setKey(config.getCryptoKey());
	//encryptorPtr_->setAlgorithm(config.getEncryptionAlgorithm());
	//encoderPtr_->setKey(config.getCryptoKey());
	//encoderPtr_->setAlgorithm(config.getEncryptionAlgorithm());
	//serializerPtr_->setKey(config.getCryptoKey());
	//serializerPtr_->setAlgorithm(config.getEncryptionAlgorithm());

	//messageConsumer_ = MessageConsumer(*encryptorPtr_, *encoderPtr_, *serializerPtr_);
	//messagePublisher_ = MessagePublisher(*encryptorPtr_, *encoderPtr_, *serializerPtr_);
	//messageHandler_ = MessageHandler(config); // config?

	messageConsumerPtr_ = std::make_unique<MessageConsumer>(*encryptorPtr_, *encoderPtr_, *serializerPtr_);
	messagePublisherPtr_ = std::make_unique<MessagePublisher>(*encryptorPtr_, *encoderPtr_, *serializerPtr_);
	//messageHandlerPtr_ = std::make_unique<MessageHandler>(*config);

}
*/

Client::Client(
		//Compressor& compressor, 
		Encryptor& encryptor, 
		Encoder& encoder, 
		Serializer& serializer, 
		C2Profile& config)
		:
		//compressor_(compressor),
		encryptor_(encryptor),
		encoder_(encoder),
		serializer_(serializer),
		config_(config)
{

}

bool Client::run(const char* host, const char* port)
{

	return true;
	//Config* config = loadConfig();
	//Config config = loadConfig();

    // 1. manually resolve apis
    /*
    TODO: 
    would i be more effient to find the function/procedure names all at once for a single dll/module so i don't have 
    to loop through the array of functions every single time?

    something like this:
    resolve_all_functions(dll_base, hashes, pointers, count);

    not sure if this is worth doing for dlls/modules tho
    
    */
}
