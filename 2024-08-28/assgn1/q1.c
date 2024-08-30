//Shresth Sonkar
//20214272
//Q1 : Concat a string to entered string using parent and child process

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h>

#define MAX_SIZE 100

int main() {
    int fd[2];
    pid_t pid;
    char inputStr[MAX_SIZE];
    printf("Enter string : ");
    scanf("%s", inputStr);
    char appendStr[] = " appended";
    char buffer[MAX_SIZE];

    if (pipe(fd) == -1) {
        perror("Pipe failed");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid > 0) {
        close(fd[0]);
        write(fd[1], inputStr, strlen(inputStr) + 1);
        close(fd[1]);
        wait(NULL);

        fd[0] = open("/dev/fd/0", O_RDONLY);
        read(fd[0], buffer, sizeof(buffer));
        printf("Concatenated String: %s\n", buffer);
        close(fd[0]);

    } else {
        close(fd[1]);
        read(fd[0], buffer, sizeof(buffer));

        int len = 0;
        while (buffer[len] != '\0') {
            len++;
        }

        int j = 0;
        while (appendStr[j] != '\0') {
            buffer[len] = appendStr[j];
            len++;
            j++;
        }
        buffer[len] = '\0';

        fd[1] = open("/dev/fd/1", O_WRONLY);
        write(fd[1], buffer, strlen(buffer) + 1);

        close(fd[0]);
        close(fd[1]);
        exit(0);
    }

    return 0;
}