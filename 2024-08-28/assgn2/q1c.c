//Q1 : There are two nodes A and B. Write a program to determine CPU load of node B from node A.

//client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    read(sock, buffer, BUFFER_SIZE);
    printf("Received CPU usage: %.2f %% ", buffer);
    close(sock);
    return 0;
}

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
//#include <arpa/inet.h>
//
//#define PORT 8080
//
//int main() {
//    int sock = 0;
//    struct sockaddr_in serv_addr;
//    double cpu_usage;
//
//    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//        perror("Socket creation error");
//        return -1;
//    }
//
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_port = htons(PORT);
//
//    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
//        perror("Invalid address/ Address not supported");
//        return -1;
//    }
//
//    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
//        perror("Connection failed");
//        return -1;
//    }
//
//    while (1) {
//        if (recv(sock, &cpu_usage, sizeof(cpu_usage), 0) > 0) {
//            printf("Received CPU usage: %.2f %% \n", cpu_usage);
//        } else {
//            perror("Receive failed");
//            close(sock);
//            return -1;
//        }
//        sleep(1);
//    }
//
//    close(sock);
//    return 0;
//}

