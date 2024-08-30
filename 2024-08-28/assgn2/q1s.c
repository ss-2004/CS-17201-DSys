//Q1 : There are two nodes A and B. Write a program to determine CPU load of node B from node A.

//server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

float get_cpu_load() {
    long double a[4], b[4], loadavg;
    FILE *fp;

    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("Failed to open /proc/stat");
        return -1;
    }

    fscanf(fp, "cpu %Lf %Lf %Lf %Lf", &a[0], &a[1], &a[2], &a[3]);
    fclose(fp);

    sleep(1);

    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("Failed to open /proc/stat");
        return -1;
    }

    fscanf(fp, "cpu %Lf %Lf %Lf %Lf", &b[0], &b[1], &b[2], &b[3]);
    fclose(fp);

    loadavg = ((b[0] + b[1] + b[2]) - (a[0] + a[1] + a[2])) /
              ((b[0] + b[1] + b[2] + b[3]) - (a[0] + a[1] + a[2] + a[3]));

    return loadavg * 100;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    printf("Server started...\n");

    if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
        perror("Accept");
        exit(EXIT_FAILURE);
    }

    float cpu_load = get_cpu_load();
    snprintf(buffer, sizeof(buffer), "Received CPU Load: %.2f%%\n", cpu_load);

    send(new_socket, buffer, strlen(buffer), 0);
    printf("Sent CPU usage: %f %% ");

    close(new_socket);
    close(server_fd);
    return 0;
}

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>
//#include <sys/time.h>
//#include <sys/resource.h>
//
//#define PORT 8080
//
//double calculate_cpu_usage() {
//    struct rusage usage;
//    getrusage(RUSAGE_SELF, &usage);
//
//    double user_time = (double)usage.ru_utime.tv_sec + (double)usage.ru_utime.tv_usec / 1e6;
//    double sys_time = (double)usage.ru_stime.tv_sec + (double)usage.ru_stime.tv_usec / 1e6;
//
//    return (user_time + sys_time)*100;
//}
//
//int main() {
//    int server_fd, new_socket;
//    struct sockaddr_in address;
//    int addrlen = sizeof(address);
//
//    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//        perror("Socket failed");
//        exit(EXIT_FAILURE);
//    }
//
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port = htons(PORT);
//
//    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//        perror("Bind failed");
//        close(server_fd);
//        exit(EXIT_FAILURE);
//    }
//    if (listen(server_fd, 3) < 0) {
//        perror("Listen failed");
//        close(server_fd);
//        exit(EXIT_FAILURE);
//    }
//
//    printf("Server is running...\n");
//
//    while (1) {
//        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
//            perror("Accept failed");
//            continue;
//        }
//
//        double cpu_usage = calculate_cpu_usage();
//        send(new_socket, &cpu_usage, sizeof(cpu_usage), 0);
//        printf("Sent CPU usage: %.2f %% \n", cpu_usage);
//
//        close(new_socket);
//        sleep(1);
//    }
//
//    close(server_fd);
//    return 0;
//}