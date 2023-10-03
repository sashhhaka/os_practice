#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

void print_process_info(char* process_name, pid_t pid, pid_t parent_pid, clock_t start_time) {
    clock_t end_time = clock();
    double execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000;

    printf("%s Process ID: %d, Parent ID: %d, Execution Time: %.3f ms\n", process_name, pid, parent_pid, execution_time);
}

int main() {
    clock_t main_start_time = clock();
    pid_t main_pid = getpid();
    pid_t pid1 = fork();
    clock_t child1_start_time = clock();

    if (!pid1) {
        // Child process 1
        pid_t child1_pid = getpid();
        pid_t child1_parent_pid = getppid();

        print_process_info("Child 1", child1_pid, child1_parent_pid, child1_start_time);
        exit(EXIT_SUCCESS);
    } else {
        // Main process
        print_process_info("Main", main_pid, getppid(), main_start_time);

        pid_t pid2 = fork();
        clock_t child2_start_time = clock();


        if (!pid2) {
            // Child process 2
            pid_t child2_pid = getpid();
            pid_t child2_parent_pid = getppid();

            print_process_info("Child 2", child2_pid, child2_parent_pid, child2_start_time);
        }
        exit(EXIT_SUCCESS);
    }

    return EXIT_SUCCESS;

}