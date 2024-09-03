//Q2 : Ricart Agrawala Algorithm
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define N 3
#define PORT_BASE 13000
#define BUF_SIZE 256

int replies_count = 0;
int my_id;
int my_port;
int request_timestamp = -1;
int has_token = 0;
int critical_section_in_use = 0;
int counter=0;
pthread_mutex_t lock;

typedef struct {
    int timestamp;
    int sender_id;
} message_t;

void enter_critical_section() {
    printf("Process %d is entering critical section.\n", my_id);
    sleep(2);
    printf("Process %d is leaving critical section.\n", my_id);
}

void send_message(int dest_id, message_t msg) {
    int sock;
    struct sockaddr_in dest_addr;
    char buffer[BUF_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT_BASE + dest_id);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Connect failed");
        close(sock);
        return;
    }

    snprintf(buffer, BUF_SIZE, "%d %d", msg.timestamp, msg.sender_id);
    send(sock, buffer, strlen(buffer), 0);
    close(sock);
}

void *listen_thread(void *arg) {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];
    socklen_t addr_len;
    message_t msg;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Set socket option failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(my_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, N) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Process %d listening on port %d...\n", my_id, my_port);

    while (counter<100) {
        counter++;
        addr_len = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        memset(buffer, 0, BUF_SIZE);
        recv(client_sock, buffer, BUF_SIZE, 0);
        sscanf(buffer, "%d %d", &msg.timestamp, &msg.sender_id);

        printf("Process %d received message from process %d with timestamp %d\n", my_id, msg.sender_id, msg.timestamp);

        pthread_mutex_lock(&lock);
        if (critical_section_in_use || (request_timestamp != -1 && (msg.timestamp > request_timestamp || (msg.timestamp == request_timestamp && msg.sender_id > my_id)))) {
            printf("Process %d delaying reply to process %d\n", my_id, msg.sender_id);
        } else {
            message_t reply = {-1, my_id};
            send_message(msg.sender_id, reply);
            printf("Process %d sent reply to process %d\n", my_id, msg.sender_id);
        }
        pthread_mutex_unlock(&lock);

        close(client_sock);
    }
}

void request_critical_section() {
    pthread_mutex_lock(&lock);
    request_timestamp = time(NULL);
    replies_count = 0;
    pthread_mutex_unlock(&lock);

    message_t msg = {request_timestamp, my_id};
    for (int i = 0; i < N; i++) {
        if (i != my_id) {
            send_message(i, msg);
        }
    }

    while (replies_count < N - 1) {
        sleep(1);
    }

    enter_critical_section();

    pthread_mutex_lock(&lock);
    critical_section_in_use = 0;
    request_timestamp = -1;
    pthread_mutex_unlock(&lock);

    for (int i = 0; i < N; i++) {
        if (i != my_id) {
            message_t reply = {-1, my_id};
            send_message(i, reply);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <process_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    my_id = atoi(argv[1]);
    my_port = PORT_BASE + my_id;

    pthread_t listener;
    pthread_mutex_init(&lock, NULL);

    if (pthread_create(&listener, NULL, listen_thread, NULL) != 0) {
        perror("Listener thread creation failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        sleep(rand() % 5 + 1);
        request_critical_section();
    }

    pthread_join(listener, NULL);
    pthread_mutex_destroy(&lock);

    return 0;
}
