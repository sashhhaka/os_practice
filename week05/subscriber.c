#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_MESSAGE_SIZE 1024

void signal_handler(int signum) {
    // Handle termination signals here
    // For example, you can close the named pipe and exit gracefully
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <subscriber_index>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int subscriber_index = atoi(argv[1]);

    // Construct the path to the named pipe based on the subscriber's index
    char pipe_name[64];
    snprintf(pipe_name, sizeof(pipe_name), "/tmp/ex1/s%d", subscriber_index);

    // Open the named pipe for reading
    int pipe_fd = open(pipe_name, O_RDONLY);
    if (pipe_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char message[MAX_MESSAGE_SIZE];
    int bytes_read;

    // Set up signal handler to handle termination signals
    signal(SIGINT, signal_handler);

    while (1) {
        bytes_read = read(pipe_fd, message, MAX_MESSAGE_SIZE);

        if (bytes_read == -1) {
            perror("read");
            break;
        } else if (bytes_read == 0) {
            // No more data to read (end of file)
            break;
        } else {
            write(STDOUT_FILENO, message, bytes_read);
        }
    }

    // Close the named pipe
    close(pipe_fd);

    return 0;
}
