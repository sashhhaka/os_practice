#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ctype.h>

#define PASSWORD_LENGTH 8

int init_files() {
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

int main() {
    // Create a file to write PID
    int urandom_fd = init_files();

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

    // Null-terminate the password and prepend "pass:"
    password[PASSWORD_LENGTH] = '\0';
    char final_password[PASSWORD_LENGTH + 6];
    snprintf(final_password, sizeof(final_password), "pass:%s", password);

    // Create a shared anonymous memory mapping
    char *shared_memory = malloc(sizeof(password));

    if (shared_memory == MAP_FAILED) {
        perror("Failed to create shared memory mapping");
        return 1;
    }

    // Copy the password to the shared memory
    strcpy(shared_memory, final_password);

    while (1) {
        sleep(1);
    }

}
