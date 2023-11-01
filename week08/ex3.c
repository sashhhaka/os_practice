#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <sys/resource.h>

#define MEMORY_SIZE 10 * 1024 * 1024
#define NUM_ALLOCS 10

int main() {
    void *memory[NUM_ALLOCS];
    struct rusage usage;
    for (int i = 0; i < NUM_ALLOCS; i++) {
        memory[i] = malloc(MEMORY_SIZE);
        memset(memory[i], 0, MEMORY_SIZE);
        getrusage(RUSAGE_SELF, &usage);
        printf("Memory usage: %ld KB\n", usage.ru_maxrss);
        sleep(1);
    }

    for (int i = 0; i < NUM_ALLOCS; i++){
        free(memory[i]);
    }
}