//Shresth Sonkar
//20214272
//Q2 : Enter matrix into parent, send to child and calculate sum. Print result on parent using pipe

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_SIZE 100

void readMatrix(int rows, int cols, int matrix[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("[%d][%d]: ", i, j);
            scanf("%d", &matrix[i][j]);
        }
    }
}

void printMatrix(int rows, int cols, int matrix[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int fd1[2], fd2[2];
    pid_t pid;

    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("pipe failed");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid > 0) {
        close(fd1[0]);
        close(fd2[1]);

        int rows, cols;
        printf("Enter the number of rows and columns of the matrices: ");
        scanf("%d %d", &rows, &cols);
        int matrix1[rows][cols];
        int matrix2[rows][cols];

        printf("Enter elements of the first matrix:\n");
        readMatrix(rows, cols, matrix1);
        printf("Enter elements of the second matrix:\n");
        readMatrix(rows, cols, matrix2);


        write(fd1[1], &rows, sizeof(int));
        write(fd1[1], &cols, sizeof(int));
        write(fd1[1], matrix1, sizeof(int) * rows * cols);
        write(fd1[1], matrix2, sizeof(int) * rows * cols);

        int result[rows][cols];
        read(fd2[0], result, sizeof(int) * rows * cols);
        printf("Sum of the matrices:\n");
        printMatrix(rows, cols, result);

        close(fd1[1]);
        close(fd2[0]);

        wait(NULL);

    } else {
        close(fd1[1]);
        close(fd2[0]);

        int rows, cols;
        read(fd1[0], &rows, sizeof(int));
        read(fd1[0], &cols, sizeof(int));

        int matrix1[rows][cols];
        int matrix2[rows][cols];
        read(fd1[0], matrix1, sizeof(int) * rows * cols);
        read(fd1[0], matrix2, sizeof(int) * rows * cols);

        int result[rows][cols];
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                result[i][j] = matrix1[i][j] + matrix2[i][j];
            }
        }

        write(fd2[1], result, sizeof(int) * rows * cols);
        close(fd1[0]);
        close(fd2[1]);
        exit(0);
    }

    return 0;
}