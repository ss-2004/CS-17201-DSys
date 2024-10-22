// Q1 : list of users who owns a file having maximum size in the current working directory using map reduce

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>

#define MAX_FILES 1024

typedef struct {
    char filename[256];
    char owner[256];
    off_t size;
} FileInfo;

FileInfo fileInfoList[MAX_FILES];
int fileCount = 0;

void map(const char *dirname) {
    struct dirent *entry;
    struct stat fileStat;
    DIR *dir = opendir(dirname);

    if (dir == NULL) {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        if (stat(path, &fileStat) == 0 && S_ISREG(fileStat.st_mode)) {
            struct passwd *pw = getpwuid(fileStat.st_uid);
            strncpy(fileInfoList[fileCount].filename, entry->d_name, 256);
            strncpy(fileInfoList[fileCount].owner, pw->pw_name, 256);
            fileInfoList[fileCount].size = fileStat.st_size;

            fileCount++;
            if (fileCount >= MAX_FILES) {
                fprintf(stderr, "Too many files, increase MAX_FILES limit.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    closedir(dir);
}

void reduce() {
    if (fileCount == 0) {
        printf("No files found in the current directory.\n");
        return;
    }

    off_t maxSize = 0;

    for (int i = 0; i < fileCount; i++) {
        if (fileInfoList[i].size > maxSize) {
            maxSize = fileInfoList[i].size;
        }
    }

    printf("Users owning files with maximum size (%lld bytes):\n", maxSize);
    for (int i = 0; i < fileCount; i++) {
        if (fileInfoList[i].size == maxSize) {
            printf("Owner: %s, File: %s\n", fileInfoList[i].owner, fileInfoList[i].filename);
        }
    }
}

int main() {
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return EXIT_FAILURE;
    }

    map(cwd);
    reduce();

    return 0;
}