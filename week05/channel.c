#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_MESSAGE_SIZE 1024

int main() {
    int pipe_fd[2]; // Pipe file descriptors: pipe_fd[0] for reading, pipe_fd[1] for writing
    pid_t child_pid;

    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // Create a child process (subscriber)
    if ((child_pid = fork()) == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // This is the child process (subscriber)
        close(pipe_fd[1]); // Close the write end of the pipe

        char message[MAX_MESSAGE_SIZE];
        int bytes_read;

        while ((bytes_read = read(pipe_fd[0], message, MAX_MESSAGE_SIZE)) > 0) {
            // Read the message from the pipe and print it to stdout
            write(STDOUT_FILENO, message, bytes_read);
        }

        close(pipe_fd[0]); // Close the read end of the pipe
    } else {
        // This is the parent process (publisher)
        close(pipe_fd[0]); // Close the read end of the pipe

        char message[MAX_MESSAGE_SIZE];

        while (1) {
            // Read a message from stdin
            if (fgets(message, MAX_MESSAGE_SIZE, stdin) == NULL) {
                break;
            }

            // Write the message to the pipe
            write(pipe_fd[1], message, strlen(message));
        }

        close(pipe_fd[1]); // Close the write end of the pipe
    }

    return 0;
}
