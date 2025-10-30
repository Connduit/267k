#include "ComponentFactory.h"



static TransportLayerUniquePtr TransportLayerFactory::create(
		MessageHandler& messageHandler, 
		const std::string& host, 
		const std::string& port, 
		TransportLayerType type)
{
	switch (type)
	{
		//case NONE:
			//break;
		case TCP:
			return std::make_unique<TCPTransportLayer>(messageHandler, host, port);
			/*
		case UDP:
			return std::make_unique<UDPTransportLayer>(messageHandler, host, port);
		case HTTP:
			return std::make_unique<HTTPTransportLayer>(messageHandler, host, port);
		case HTTPS:
			return std::make_unique<HTTPSTransportLayer>(messageHandler, host, port);
		case DNS:
			return std::make_unique<DNSTransportLayer>(messageHandler, host, port);
			*/
		default:
			// throw error
			break;
}
