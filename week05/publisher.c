#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_MESSAGE_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_subscribers>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create a directory for named pipes if it doesn't exist
    const char* dir_path = "/tmp/ex1";
    if (mkdir(dir_path, 0777) == -1 && errno != EEXIST) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }

    // Create named pipes for subscribers
    int num_subscribers = atoi(argv[1]); // Change this to the desired number of subscribers
    char pipe_names[num_subscribers][64];
    int pipe_fds[num_subscribers];

    for (int i = 0; i < num_subscribers; i++) {
        snprintf(pipe_names[i], sizeof(pipe_names[i]), "%s/s%d", dir_path, i + 1);
        mkfifo(pipe_names[i], 0666);

        // Open the named pipes for writing
        pipe_fds[i] = open(pipe_names[i], O_WRONLY);
        if (pipe_fds[i] == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
    }

    char message[MAX_MESSAGE_SIZE];

    // Continuously read messages from stdin and send them to subscribers
    while (1) {
        if (fgets(message, MAX_MESSAGE_SIZE, stdin) == NULL) {
            break;
        }

        // Send the message to each subscriber via their named pipe
        for (int i = 0; i < num_subscribers; i++) {
            write(pipe_fds[i], message, strlen(message));
        }
    }

    // Close all pipe file descriptors
    for (int i = 0; i < num_subscribers; i++) {
        close(pipe_fds[i]);
    }

    return 0;
}
