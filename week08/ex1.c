#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ctype.h>

#define PASSWORD_LENGTH 8

int initFiles() {
    // Create a file to write PID
    FILE *pidFile = fopen("/tmp/ex1.pid", "w");
    if (pidFile != NULL) {
        fprintf(pidFile, "%d", getpid());
        fclose(pidFile);
    } else {
        perror("Failed to open ex1.pid");
    }

    // Open /dev/urandom to generate a random password
    int urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd == -1) {
        perror("Failed to open /dev/urandom");
        exit(EXIT_FAILURE);
    }
    return urandom_fd;
}

char* generateRandomPassword(int urandom_fd) {
    char password[PASSWORD_LENGTH + 1];
    char buffer[PASSWORD_LENGTH];
    int buffer_index = 0;

    for (int i = 0; i < PASSWORD_LENGTH; i++) {
        if (!buffer_index) {
            ssize_t bytes_read = read(urandom_fd, buffer, PASSWORD_LENGTH);
            if (bytes_read != -1) {
                buffer_index = bytes_read;
            } else {
                perror("read");
                exit(1);
            }
        }

        char symbol = buffer[--buffer_index];
        isprint(symbol) ? password[i] = symbol : i--;
    }

    close(urandom_fd);
    password[PASSWORD_LENGTH] = '\0';

    // Return a dynamically allocated copy of the password
    return strdup(password);
}

void createSharedMemory(const char* password) {
    // Create a shared anonymous memory mapping
    char *shared_memory = malloc(strlen(password) + 6);

    if (shared_memory == NULL) {
        perror("Failed to create shared memory mapping");
        exit(1);
    }

    // Null-terminate the password and prepend "pass:"
    snprintf(shared_memory, strlen(password) + 6, "pass:%s", password);

    // Copy the password to the shared memory
    strcpy(shared_memory, shared_memory);
}

int main() {
    // Create a file to write PID and open /dev/urandom
    int urandom_fd = initFiles();

    char *password = generateRandomPassword(urandom_fd);
    createSharedMemory(password);

    while (1) {
        sleep(1);
    }

    // Don't forget to free the dynamically allocated memory when done with password and shared_memory.
    free(password);
}
