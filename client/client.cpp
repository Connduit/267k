#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "10.0.0.86"  // Localhost for testing
#define SERVER_PORT 4444
#define BUFFER_SIZE 1024

// Command structure
typedef struct {
    char action[50];
    char target[100];
    char options[200];
} Command;

// Parse commands from server
Command parse_command(char* buffer) {
    Command cmd = {0};
    
    // Simple parsing: "action:target:options"
    char* token = strtok(buffer, ":");
    if (token) strncpy(cmd.action, token, sizeof(cmd.action) - 1);
    
    token = strtok(NULL, ":");
    if (token) strncpy(cmd.target, token, sizeof(cmd.target) - 1);
    
    token = strtok(NULL, "\n");
    if (token) strncpy(cmd.options, token, sizeof(cmd.options) - 1);
    
    return cmd;
}

// Execute commands
void execute_command(Command cmd) {
    printf("Executing: %s -> %s [%s]\n", cmd.action, cmd.target, cmd.options);
    
    if (strcmp(cmd.action, "download") == 0) {
        printf("[+] Downloading: %s\n", cmd.target);
    }
    else if (strcmp(cmd.action, "upload") == 0) {
        printf("[+] Uploading: %s\n", cmd.target);
    }
    else if (strcmp(cmd.action, "execute") == 0) {
        printf("[+] Executing: %s\n", cmd.target);
    }
    else if (strcmp(cmd.action, "systeminfo") == 0) {
        printf("[+] Gathering system information...\n");
    }
    else if (strcmp(cmd.action, "persist") == 0) {
        printf("[+] Establishing persistence...\n");
    }
    else {
        printf("[-] Unknown command: %s\n", cmd.action);
    }
}

// Send response back to server
void send_response(SOCKET sock, char* message) {
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "RESPONSE:%s\n", message);
    send(sock, response, strlen(response), 0);
}

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    
    WSAStartup(MAKEWORD(2,2), &wsa);
    
    while (1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_addr.s_addr = inet_addr(SERVER_IP);
        server.sin_family = AF_INET;
        server.sin_port = htons(SERVER_PORT);
        
        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
            printf("[-] Connection failed, retrying in 10s...\n");
            closesocket(sock);
            Sleep(10000);
            continue;
        }
        
        printf("[+] Connected to server\n");
        send(sock, "READY\n", 6, 0);
        
        // Command loop
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int size = recv(sock, buffer, sizeof(buffer) - 1, 0);
            
            if (size <= 0) {
                printf("[-] Connection lost\n");
                break;
            }
            
            buffer[size] = '\0';
            printf("[+] Received: %s", buffer);
            
            // Parse and execute command
            Command cmd = parse_command(buffer);
            execute_command(cmd);
            
            // Send acknowledgement
            send_response(sock, "Command executed");
        }
        
        closesocket(sock);
        Sleep(5000); // Wait before reconnecting
    }
    
    WSACleanup();
    return 0;
}
