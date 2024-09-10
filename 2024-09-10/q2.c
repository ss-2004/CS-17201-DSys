//Q2 : Map Reduce for file owner with max size

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>

struct file_info {
    char owner[256];
    off_t size;
};

void get_file_owner(uid_t uid, char *owner_name) {
    struct passwd *pwd = getpwuid(uid);
    if (pwd) {
        strcpy(owner_name, pwd->pw_name);
    } else {
        strcpy(owner_name, "Unknown");
    }
}

void map_files(const char *directory, struct file_info *file_list, int *count) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char filepath[1024];

    if ((dir = opendir(directory)) == NULL) {
        perror("opendir() error");
        exit(EXIT_FAILURE);
    }

    *count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

        if (stat(filepath, &file_stat) == 0) {
            if (S_ISREG(file_stat.st_mode)) {
                get_file_owner(file_stat.st_uid, file_list[*count].owner);
                file_list[*count].size = file_stat.st_size;
                (*count)++;
            }
        }
    }
    closedir(dir);
}

void reduce_files(struct file_info *file_list, int count) {
    off_t max_size = 0;

    for (int i = 0; i < count; i++) {
        if (file_list[i].size > max_size)
            max_size = file_list[i].size;
    }

    printf("User(s) owning file(s) with maximum size %lld bytes:\n", (long long) max_size);
    for (int i = 0; i < count; i++) {
        if (file_list[i].size == max_size)
            printf("%s\n", file_list[i].owner);
    }
}

int main() {
    const char *directory = ".";
    struct file_info file_list[1024];
    int file_count = 0;

    map_files(directory, file_list, &file_count);

    if (file_count > 0)
        reduce_files(file_list, file_count);
    else
        printf("No files found in the directory.\n");


    return 0;
}