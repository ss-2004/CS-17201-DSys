//Write a client (UDP) C program that calls sendto() to send string to server program knowing IP address and port number.
//client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SERVER_PORT 12345
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed!\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("invalid address or address not supported!\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter the message to send: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len) < 0) {
        perror("sendto failed");
        exit(EXIT_FAILURE);
    }

    printf("Message sent to server\n");
    close(sockfd);
    return 0;
}