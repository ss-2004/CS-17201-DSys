// Q2 : RPC Server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void send_file(FILE *file, int socket_fd) {
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        send(socket_fd, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);
    }
    printf("File sent successfully.\n");
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connections...\n");

    new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    char filename[BUFFER_SIZE] = {0};
    recv(new_socket, filename, BUFFER_SIZE, 0);
    printf("Client requested file: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("File not found");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    send_file(file, new_socket);
    fclose(file);
    close(new_socket);
    close(server_fd);

    return 0;
}