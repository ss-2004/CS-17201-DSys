//Q2 : Write a server C program using shared memory and semaphore (server increments counter between sem_wait) and sem_post()).
// Create shared memory using mmap.

//q2s
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

    fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, SHARED_MEM_SIZE) == -1) {
        perror("ftruncate failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    shared_counter = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_counter == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    *shared_counter = 0;

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        munmap(shared_counter, SHARED_MEM_SIZE);
        close(fd);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 10; i++) {
        sem_wait(sem);

        (*shared_counter)++;
        printf("Counter value: %d\n", *shared_counter);

        sem_post(sem);

        sleep(1);
    }

    sem_close(sem);
    sem_unlink(SEM_NAME);
    munmap(shared_counter, SHARED_MEM_SIZE);
    close(fd);
    shm_unlink("/my_shared_memory");
    return 0;
}