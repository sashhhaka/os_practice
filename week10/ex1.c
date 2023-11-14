#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

struct stat fileStat;


// Function to find all hard links to the given source file
void find_all_hlinks(const char *source, const char *watched_dir) {
//    struct stat fileStat;
    char *path = malloc(strlen(watched_dir) + strlen(source) + 2);
    strcpy(path, watched_dir);
    strcat(path, "/");
    strcat(path, source);

    if (stat(path, &fileStat) < 0) {
        printf("Path: %s\n", path);
        perror("Error getting file stat");
        exit(EXIT_FAILURE);
    }

    printf("Hard links to %s (inode %lu):\n", source, (unsigned long)fileStat.st_ino);

    // Open the directory for reading
    DIR *dir = opendir(watched_dir);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Check if it's a regular file
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", watched_dir, entry->d_name);

            struct stat entryStat;
            if (lstat(path, &entryStat) < 0) {
                perror("Error getting file stat");
                exit(EXIT_FAILURE);
            }

            if (entryStat.st_ino == fileStat.st_ino) {
                char resolved_path[PATH_MAX];
                if (realpath(path, resolved_path) == NULL) {
                    perror("Error getting realpath");
                    exit(EXIT_FAILURE);
                }

                printf("Inode: %lu, Path: %s\n", (unsigned long)entryStat.st_ino, resolved_path);
                printf("Content: ");
                // print the content of the file line by line
                FILE *fp = fopen(resolved_path, "r");
                if (fp == NULL) {
                    perror("Error opening file");
                    exit(EXIT_FAILURE);
                }

                char *line = NULL;
                size_t len = 0;
                ssize_t read;
                while ((read = getline(&line, &len, fp)) != -1) {
                    printf("%s", line);
                }
                printf("\n");
                fclose(fp);
            }
        }
    }

    closedir(dir);
}

// Function to unlink all duplicates of a hard link, keeping only one
void unlink_all(const char *source, const char *watched_dir) {
    struct stat fileStat;
    char *path = malloc(strlen(watched_dir) + strlen(source) + 2);
    strcpy(path, watched_dir);
    strcat(path, "/");
    strcat(path, source);

    if (stat(path, &fileStat) < 0) {
        perror("Error getting file stat");
        exit(EXIT_FAILURE);
    }

    // Open the directory for reading
    DIR *dir = opendir(watched_dir);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Check if it's a regular file
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", watched_dir, entry->d_name);

            // if the file is the same as the source file, skip it
            // compare entry with source
            if (strcmp(entry->d_name, source) == 0) continue;

            struct stat entryStat;
            if (lstat(path, &entryStat) < 0) {
                perror("Error getting file stat");
                exit(EXIT_FAILURE);
            }

            if (entryStat.st_ino == fileStat.st_ino && strcmp(path, source) != 0) {
                if (unlink(path) < 0) {
                    perror("Error unlinking file");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    closedir(dir);
}

// Function to create a symbolic link
void create_sym_link(const char *source, const char *link, const char *watched_dir) {


    if (symlink(source, link) < 0) {
        perror("Error creating symbolic link");
        exit(EXIT_FAILURE);
    }
}

void print_stat_info(const char *file, const char *watched_dir) {
    // concatenate the path
    char *path = malloc(strlen(watched_dir) + strlen(file) + 2);
    strcpy(path, watched_dir);
    strcat(path, "/");
    strcat(path, file);

    if (stat(path, &fileStat) < 0) {
        printf("--------------\n");
        return;
    }


    printf("File/Directory: %s\n", path);
    printf("Size: %ld bytes, Blocks: %ld, IO Block: %ld\n", fileStat.st_size, fileStat.st_blocks, fileStat.st_blksize);
    printf("Device: %ld, Inode: %ld, Links: %ld\n", fileStat.st_dev, fileStat.st_ino, fileStat.st_nlink);
    //

    printf("Access: %o\n", fileStat.st_mode);
    printf("Access: %s", ctime(&fileStat.st_atime));
    printf("Modify: %s", ctime(&fileStat.st_mtime));
    printf("Change: %s", ctime(&fileStat.st_ctime));
    printf("--------------\n");
}


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <watched_directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *watched_dir = argv[1];

    // Create a file myfile1.txt and two hard links to it in the watched directory
    char *file1 = "myfile1.txt";
    char *path1 = malloc(strlen(watched_dir) + strlen(file1) + 2);
    strcpy(path1, watched_dir);
    strcat(path1, "/");
    strcat(path1, file1);

    char *file12 = "myfile12.txt";
    char *path12 = malloc(strlen(watched_dir) + strlen(file12) + 2);
    strcpy(path12, watched_dir);
    strcat(path12, "/");
    strcat(path12, file12);

    char *file11 = "myfile11.txt";
    char *path11 = malloc(strlen(watched_dir) + strlen(file11) + 2);
    strcpy(path11, watched_dir);
    strcat(path11, "/");
    strcat(path11, file11);

    char *file13 = "myfile13.txt";
    char *path13 = malloc(strlen(watched_dir) + strlen(file13) + 2);
    strcpy(path13, watched_dir);
    strcat(path13, "/");
    strcat(path13, file13);

    int file_fd = open(path1, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_fd < 0) {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }

    write(file_fd, "Hello world.", 12);
    close(file_fd);

    link(path1, path11);
    link(path1, path12);

    // Find all hard links to myfile1.txt and print their i-nodes, paths, and content
    find_all_hlinks("myfile1.txt", watched_dir);

    // Move myfile1.txt to another folder
    rename(path1, "/tmp/myfile1.txt");


    // Modify the file myfile11.txt (its content)
    int file11_fd = open(path11, O_WRONLY | O_APPEND);
    if (file11_fd < 0) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    write(file11_fd, " Modify myfile11.txt.", 22);
    close(file11_fd);

    // Create a symbolic link myfile13.txt in the watched directory to /tmp/myfile1.txt
    create_sym_link("/tmp/myfile1.txt", path13, watched_dir);

    // Modify the file /tmp/myfile1.txt (its content)
    int tmp_file_fd = open("/tmp/myfile1.txt", O_WRONLY | O_APPEND);
    if (tmp_file_fd < 0) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    write(tmp_file_fd, " Modify /tmp/myfile1.txt.", 25);
    close(tmp_file_fd);

    // Remove all duplicates of hard links to myfile11.txt only in the watched directory
    unlink_all("myfile11.txt", watched_dir);

    // find the kept hard link and print the stat info about it
    find_all_hlinks("myfile11.txt", watched_dir);
    print_stat_info("myfile11.txt", watched_dir);


    return EXIT_SUCCESS;
}

