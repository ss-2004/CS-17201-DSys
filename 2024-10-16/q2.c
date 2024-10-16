//Load Balancer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int get_cpu_load(const char *ip, int port) {
    return rand() % 100;
}

void forward_message(const char *ip, int port, const char *message, char *response) {
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_address.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("Connection to server failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    send(sock, message, strlen(message), 0);

    read(sock, buffer, BUFFER_SIZE);
    strcpy(response, buffer);
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server1_port> <server2_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server1_port = atoi(argv[1]);
    int server2_port = atoi(argv[2]);

    if (server1_port <= 0 || server2_port <= 0) {
        fprintf(stderr, "Invalid port numbers.\n");
        exit(EXIT_FAILURE);
    }

    int load_balancer_sock, client_sock;
    struct sockaddr_in load_balancer_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    char buffer[BUFFER_SIZE] = {0};
    const char *server_ip = "127.0.0.1";

    if ((load_balancer_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    load_balancer_address.sin_family = AF_INET;
    load_balancer_address.sin_addr.s_addr = INADDR_ANY;
    load_balancer_address.sin_port = htons(8080);

    if (bind(load_balancer_sock, (struct sockaddr *) &load_balancer_address, sizeof(load_balancer_address)) < 0) {
        perror("Bind failed");
        close(load_balancer_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(load_balancer_sock, 5) < 0) {
        perror("Listen failed");
        close(load_balancer_sock);
        exit(EXIT_FAILURE);
    }

    printf("Load balancer listening on port 8080...\n");

    while (1) {
        client_sock = accept(load_balancer_sock, (struct sockaddr *) &client_address, &client_address_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);
        read(client_sock, buffer, BUFFER_SIZE);
        printf("Received message from client: %s\n", buffer);

        int cpu_load1 = get_cpu_load(server_ip, server1_port);
        int cpu_load2 = get_cpu_load(server_ip, server2_port);
        printf("CPU Load - Server 1: %d%%, Server 2: %d%%\n", cpu_load1, cpu_load2);

        int selected_port = (cpu_load1 <= cpu_load2) ? server1_port : server2_port;
        printf("Forwarding message to server on port %d...\n", selected_port);

        char response[BUFFER_SIZE] = {0};
        forward_message(server_ip, selected_port, buffer, response);
        send(client_sock, response, strlen(response), 0);
        close(client_sock);
    }

    close(load_balancer_sock);
    return 0;
}