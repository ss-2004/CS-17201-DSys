// Echo client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Type a message and press Enter:\n");

    while (1) {
        printf("Message: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        send(client_socket, buffer, strlen(buffer), 0);

        if ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
            buffer[bytes_received] = '\0';
            printf("Echo from server: %s\n", buffer);
        }
    }

    close(client_socket);
    return 0;
}