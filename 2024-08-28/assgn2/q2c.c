//Q2: Write a client C program that reads counter value between sem _wait and sem_post.
// Access shared memory using open ().

//q2c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define SHARED_MEM_SIZE sizeof(int)
#define SEM_NAME "/my_semaphore"

int main() {
    int fd;
    int *shared_counter;
    sem_t *sem;

    fd = shm_open("/my_shared_memory", O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    shared_counter = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_counter == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        munmap(shared_counter, SHARED_MEM_SIZE);
        close(fd);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 10; i++) {
        sem_wait(sem);
        printf("Client: Counter value is %d\n", *shared_counter);
        sem_post(sem);
        sleep(1);
    }

    sem_close(sem);
    munmap(shared_counter, SHARED_MEM_SIZE);
    close(fd);
    return 0;
}