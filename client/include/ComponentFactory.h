
/* ComponentFactory.h */

// TODO: factory should take in a config file? either from C2Profile
// or maybe even directly from a provided json file? 


#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H


//#include "Compressor.h"
#include "Encoder.h"
#include "Encryptor.h"
#include "Serializer.h"
#include "TransportLayer.h"
#include "TransportLayerTypes.h"


#include <memory>

class ComponentFactory
{
public:
private:
};

class TransportLayerFactory
{
public:
	static TransportLayerUniquePtr create(
			MessageHandler& messageHandler, 
			const std::string& host, 
			const std::string& port, 
			TransportLayerType type);
private:
};



/*
// You need to create an entire system with dependencies
class MessageSystemFactory {
	static std::unique_ptr<MessageSystem> createSystem(const SystemConfig& config) {
		auto encoder = EncoderFactory::createEncoder(config.encoder);
		auto compressor = CompressorFactory::createCompressor(config.compressor);
		auto network = NetworkFactory::createNetwork(config.network);

		// Wire them together properly
		return std::make_unique<MessageSystem>(encoder, compressor, network);
	}
};




// Individual factories (Factory Pattern)
class EncryptorFactory {   };
class EncoderFactory { };
class TransportFactory {  };

// Component factory that uses them (Component Factory)
class MessageClientFactory 
{
public:
	static std::unique_ptr<MessageClient> create(const ClientConfig& config) 
	{
		return std::make_unique<MessageClient>(
					EncryptorFactory::create(config.encryptor),
					EncoderFactory::create(config.encoder),
					TransportFactory::create(config.transport)
					);
	}
};
*/

class EncryptorFactory
{
public:
private:
};
class EncoderFactory
{
public:
private:
};
class SerializerFactory
{
public:
private:
};
/*
class CompressorFactory
{   
};
class TransportFactory
{   
};
*/



#endif
