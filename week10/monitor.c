#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))
#define PATH_MAX 4096

struct stat fileStat;


void print_stat_info(const char *file, const char *watched_dir) {
    // concatenate the path
    char *path = malloc(strlen(watched_dir) + strlen(file) + 2);
    strcpy(path, watched_dir);
    strcat(path, "/");
    strcat(path, file);


//    struct stat fileStat;
    if (stat(path, &fileStat) < 0) {
        printf("--------------\n");
        return;
    }

    printf("File/Directory: %s\n", path);
    printf("Size: %ld bytes, Blocks: %ld, IO Block: %ld\n", fileStat.st_size, fileStat.st_blocks, fileStat.st_blksize);
    printf("Device: %ld, Inode: %ld, Links: %ld\n", fileStat.st_dev, fileStat.st_ino, fileStat.st_nlink);

    printf("Access: %o\n", fileStat.st_mode);
    printf("Access: %s", ctime(&fileStat.st_atime));
    printf("Modify: %s", ctime(&fileStat.st_mtime));
    printf("Change: %s", ctime(&fileStat.st_ctime));
    printf("--------------\n");
}

void handle_sigint(int signum) {
    printf("\nReceived SIGINT. Printing stat info before termination.\n");
    // Print stat info for all files and directories in the specified path
    // You may need to modify the path based on your program structure
    // For example: print_stat_info("/path/to/directory/file.txt");
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Register the signal handler for SIGINT
    signal(SIGINT, handle_sigint);

    // Get the directory path from the command line argument
    char *dir_path = argv[1];


    // Initialize inotify
    int fd = inotify_init();
    if (fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    // Add a watch to the specified directory for specified events
    int wd = inotify_add_watch(fd, dir_path,
                               IN_ACCESS | IN_CREATE | IN_DELETE | IN_MODIFY | IN_OPEN);
    if (wd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    char buffer[EVENT_BUF_LEN];

    // Monitor changes in the directory
    while (1) {
        int length = read(fd, buffer, EVENT_BUF_LEN);

        if (length < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];

            if (event->len) {
                if (event->mask & IN_ACCESS)
                    printf("File accessed: %s\n", event->name);
                if (event->mask & IN_CREATE)
                    printf("File/directory created: %s\n", event->name);
                if (event->mask & IN_DELETE)
                    printf("File/directory deleted: %s\n", event->name);
                if (event->mask & IN_MODIFY)
                    printf("File modified: %s\n", event->name);
                if (event->mask & IN_OPEN)
                    printf("File/directory opened: %s\n", event->name);
                if (event->mask & IN_ATTRIB)
                    printf("Metadata changed: %s\n", event->name);
                // Print stat info for the changed item
                char path[PATH_MAX];
                snprintf(path, PATH_MAX, "%s/%s", dir_path, event->name);
                print_stat_info(event->name, dir_path);
            }
            i += EVENT_SIZE + event->len;
        }
    }

    // This part is not reached because the program is terminated by SIGINT
    // Clean up and close inotify


    return EXIT_SUCCESS;
}

