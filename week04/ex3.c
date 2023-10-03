#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Invalid number of arguments.\n");
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    if (n < 1) {
        printf("Invalid number of processes, choose number >= 1.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            printf("Child process %d\n", getpid());
            sleep(5);
            exit(EXIT_SUCCESS);
        }
    }

    return EXIT_SUCCESS;
}


