//Design a Distributed Application using Message Passing Interface (MPI) for remote computation.
//server
//mpicc q1s.c -o q1s
//mpirun -n 5 ./q1s
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mpi.h>

#define UDP_PORT 12345
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Status status;
    char buffer[BUFFER_SIZE];
    int udp_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("MPI server (rank %d) listening for UDP messages on port %d\n", rank, UDP_PORT);
        if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("UDP socket creation failed!\n");
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(UDP_PORT);

        if (bind(udp_socket, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("UDP socket bind failed!\n");
            close(udp_socket);
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }

        while (1) {
            int n = recvfrom(udp_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
            if (n < 0) {
                perror("UDP recvfrom failed!\n");
                break;
            }

            buffer[n] = '\0';
            printf("\nReceived UDP message: %s\n", buffer);

            int result = strlen(buffer);
            MPI_Bcast(&result, 1, MPI_INT, 0, MPI_COMM_WORLD);
            printf("Broadcasted result to all MPI processes: %d\n", result);
        }
        close(udp_socket);
    } else {
        int received_result;
        MPI_Bcast(&received_result, 1, MPI_INT, 0, MPI_COMM_WORLD);
        printf("MPI process (rank %d) received broadcast result: %d\n", rank, received_result);
    }

    MPI_Finalize();
    return 0;
}