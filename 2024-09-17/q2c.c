// master routing table client

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 9090
#define SERVER_IP "127.0.0.1"

typedef struct {
    int node_no;
    char ip_address[INET_ADDRSTRLEN];
    int port_no;
} ClientInfo;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    ClientInfo master_table[10];
    int num_entries;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    read(sock, &num_entries, sizeof(num_entries));
    printf("Received master table with %d entries:\n", num_entries);
    for (int i = 0; i < num_entries; i++) {
        read(sock, &master_table[i], sizeof(master_table[i]));
        printf("Node %d: IP %s, Port %d\n", master_table[i].node_no, master_table[i].ip_address,
               master_table[i].port_no);
    }

    close(sock);
    return 0;
}