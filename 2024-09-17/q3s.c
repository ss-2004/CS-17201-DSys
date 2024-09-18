// Date time CPU load server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//#ifdef __APPLE__
#include <mach/mach.h>
//#elif __linux__
//#include <linux/limits.h>
//#endif

#define PORT 8080
#define BUFFER_SIZE 1024

void get_datetime(char *buffer) {
    time_t t;
    struct tm *tmp;
    char time_str[64];

    t = time(NULL);
    tmp = localtime(&t);

    if (tmp == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

    if (strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    }

    strcpy(buffer, time_str);
}

//#ifdef __APPLE__
void get_cpu_load_macos(char *buffer) {
    natural_t cpu_count;
    processor_info_array_t cpu_info;
    mach_msg_type_number_t num_cpu_info;
    kern_return_t kr;

    kr = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &cpu_count, &cpu_info, &num_cpu_info);
    if (kr != KERN_SUCCESS) {
        strcpy(buffer, "Failed to get CPU load info");
        return;
    }

    unsigned long long total_ticks = 0, idle_ticks = 0;
    for (unsigned int i = 0; i < cpu_count; i++) {
        natural_t *cpu_load = (natural_t *)&cpu_info[i * CPU_STATE_MAX];
        idle_ticks += cpu_load[CPU_STATE_IDLE];
        for (int j = 0; j < CPU_STATE_MAX; j++) {
            total_ticks += cpu_load[j];
        }
    }

    vm_deallocate(mach_task_self(), (vm_address_t)cpu_info, num_cpu_info * sizeof(natural_t));

    double cpu_usage = (1.0 - ((double)idle_ticks / total_ticks)) * 100.0;
    sprintf(buffer, "CPU Load (macOS): %.2f%%", cpu_usage);
}
//#endif

//#ifdef __linux__
//void get_cpu_load_linux(char *buffer) {
//    FILE *fp;
//    char line[128];
//    long double a[4], b[4], load;
//
//    fp = fopen("/proc/stat", "r");
//    if (fp == NULL) {
//        perror("fopen");
//        exit(EXIT_FAILURE);
//    }
//
//    fgets(line, sizeof(line), fp);
//    sscanf(line, "cpu %Lf %Lf %Lf %Lf", &a[0], &a[1], &a[2], &a[3]);
//    fclose(fp);
//    sleep(1);
//
//    fp = fopen("/proc/stat", "r");
//    fgets(line, sizeof(line), fp);
//    sscanf(line, "cpu %Lf %Lf %Lf %Lf", &b[0], &b[1], &b[2], &b[3]);
//    fclose(fp);
//
//    load = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
//    sprintf(buffer, "CPU Load (Linux): %.2Lf%%", load * 100);
//}
//#endif
//
//void get_cpu_load(char *buffer) {
//#ifdef __APPLE__
//    get_cpu_load_macos(buffer);
//#elif __linux__
//    get_cpu_load_linux(buffer);
//#else
//    strcpy(buffer, "Unsupported platform");
//#endif
//}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char datetime[64], cpu_load[64];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt SO_REUSEADDR failed");
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
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Waiting for a client to connect...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Client connected.\n");
        get_datetime(datetime);
        get_cpu_load_macos(cpu_load);

        snprintf(buffer, sizeof(buffer), "Date-Time: %s\n%s\n", datetime, cpu_load);
        send(new_socket, buffer, strlen(buffer), 0);
        printf("Sent to client:\n%s", buffer);

        close(new_socket);
    }
    return 0;
}