#ifndef CLIENT_H
#define CLIENT_H


#include "C2Profile.h"
#include "MessageHandler.h"
#include "Transporter.h"
#include "../common/logging.h"

#include <memory>


class Client
{
public:
    Client();
    //Client(std::string server, std::string port); 
    Client(TCPTransporter transporter, std::string server, std::string port); 

    bool run();
private:

    //Transporter transporter_;
    TCPTransporter transporter_; // NOTE: hardcode transport type?, TODO: eventually turn this into a "ConnectionManager" that can own multiple transporters
    MessageHandler messageHandler_;
	// ApiResolver apiResolver_;


    //C2Profile& config_; // TODO: eventually have 

    // TODO: Eventually have multiple transporters if i need to connect to multiple servers.
    //       This allows us to act as both cnc and p2p?
    // std::vector<std::unique_ptr<Transporter>> transporters_;

};


#endif // CLIENT_H


