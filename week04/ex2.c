#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    FILE *fp = fopen("temp.txt", "w+");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return EXIT_FAILURE;
    }
    int u[120], v[120];
    for (int i = 0; i < 120; i++) {
        u[i] = rand() % 100;
        v[i] = rand() % 100;
    }


    int n;
    printf("Enter number of processes: ");
    scanf("%d", &n);
    if ((n < 1) || (n > 120)) {
        printf("Invalid number of processes, choose number from 1 to 120.\n");
        return EXIT_FAILURE;
    }

    pid_t pids[n];
    int block_size = 120 / n;
    int remainder = 120 % n;

    // distribute the remainder among processes equally
    // for example, when the remainder is 2, first 2 processes are going to execute block_size + 1 elements
    // and the rest of the processes are going to execute block_size elements
    int start = 0;
    int end = 0;
    for (int i = 0; i < n; i++) {
        if (remainder > 0) {
            end = start + block_size + 1;
            remainder--;
        } else {
            end = start + block_size;
        }
        pids[i] = fork();
        if (pids[i] == 0) {
            // Child process
            int sum = 0;
            for (int j = start; j < end; j++) {
                sum += u[j] * v[j];
            }
            fprintf(fp, "%d\n", sum);
            exit(EXIT_SUCCESS);
        }
        start = end;
    }


    // Wait for all child processes to finish
    for (int i = 0; i < n; i++) {
        waitpid(pids[i], NULL, 0);
    }

    // Read the file and calculate the final result
    rewind(fp);
    long sum = 0;
    int temp;
    while (fscanf(fp, "%d", &temp) != EOF) {
        sum += temp;
    }
    printf("Dot product: %ld\n", sum);

    fclose(fp);
    return EXIT_SUCCESS;
}

