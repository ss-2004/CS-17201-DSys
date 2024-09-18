// master routing table server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9090
#define MAX_CLIENTS 10

typedef struct {
    int node_no;
    char ip_address[INET_ADDRSTRLEN];
    int port_no;
} ClientInfo;

ClientInfo master_table[MAX_CLIENTS];
int num_clients = 0;

void add_client_to_master(int client_sock, struct sockaddr_in client_addr) {
    inet_ntop(AF_INET, &client_addr.sin_addr, master_table[num_clients].ip_address, INET_ADDRSTRLEN);
    master_table[num_clients].port_no = ntohs(client_addr.sin_port);
    master_table[num_clients].node_no = num_clients + 1;
    num_clients++;
}

void send_master_table(int client_sock) {
    write(client_sock, &num_clients, sizeof(num_clients));
    for (int i = 0; i < num_clients; i++) {
        write(client_sock, &master_table[i], sizeof(master_table[i]));
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);

    while ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen))) {
        printf("Connection established from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        add_client_to_master(new_socket, address);
        send_master_table(new_socket);
        close(new_socket);
    }

    return 0;
}