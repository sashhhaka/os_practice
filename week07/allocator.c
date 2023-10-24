// simple memory allocator  simulation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define MAX_MEM 10000000
#define MAX_MEM 10000000

// Define global variables
clock_t start_time;

// Function to reset the timer
void reset_timer() {
    start_time = clock();
}

// Function to get the elapsed time in seconds
double elapsed_time() {
    clock_t end_time = clock();
    return ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
}

unsigned int mem[MAX_MEM];
// set all the cells of the memory to zero


void allocate_first_fit(unsigned int adrs, unsigned int size) {
    int i = 0;
    int j = 0; // Initialize j to the current position
    int free_block_size = 0; // Track the size of the free block

    while (j < MAX_MEM) {
        if (mem[j] == 0) {
            free_block_size++;
            if (free_block_size >= size) {
                // Found a contiguous block of free memory of size at least size
                for (int k = i; k < i + size; k++) {
                    mem[k] = adrs;
                }
                return;
            }
        } else {
            free_block_size = 0;
            i = j + 1; // Move the start position to the next cell
        }
        j++; // Move the current position to the next cell
    }
}



void allocate_best_fit(unsigned int adrs, unsigned int size) {
    int best_fit_index = -1;
    int best_fit_size = MAX_MEM;
    int current_block_size = 0;

    for (int i = 0; i < MAX_MEM; i++) {
        if (mem[i] == 0) {
            current_block_size++;

            if (current_block_size >= size) {
                // Found a block of sufficient size
                if (current_block_size < best_fit_size) {
                    best_fit_size = current_block_size;
                    best_fit_index = i - current_block_size + 1;
                }
            }
        } else {
            current_block_size = 0;
        }
    }

    if (best_fit_index != -1) {
        // Allocate the best fit block
        for (int k = best_fit_index; k < best_fit_index + size; k++) {
            mem[k] = adrs;
        }
    }
}


void allocate_worst_fit(unsigned int adrs, unsigned int size) {
    int worst_fit_index = -1;
    int worst_fit_size = 0; // Initialize to a small value

    int current_block_size = 0;

    for (int i = 0; i < MAX_MEM; i++) {
        if (mem[i] == 0) {
            current_block_size++;

            if (current_block_size >= size) {
                // Found a block of sufficient size
                if (current_block_size > worst_fit_size) {
                    worst_fit_size = current_block_size;
                    worst_fit_index = i - current_block_size + 1;
                }
            }
        } else {
            current_block_size = 0;
        }
    }

    if (worst_fit_index != -1) {
        // Allocate memory in the worst fit block
        for (int k = worst_fit_index; k < worst_fit_index + size; k++) {
            mem[k] = adrs;
        }
    }
}

void clear(unsigned int adrs) {
    // clear the memory cells that are set to adrs
    for (int i = 0; i < MAX_MEM; i++) {
        if (mem[i] == adrs) {
            mem[i] = 0;
        }
    }
}

void reset() {
    // reset the memory
    memset(mem, 0, MAX_MEM * sizeof(unsigned int));
}

void execute(char *algorithm) {
    reset();

    FILE *file = fopen("queries.txt", "r");
    if (file == NULL) {
        printf("File is not found or cannot open it!\n");
        exit(EXIT_FAILURE);
    }
    char command[10];
    unsigned int adrs;
    unsigned int size;
    int query_count = 0;
    reset_timer();
    while (fscanf(file, "%s %u %u", command, &adrs, &size) != EOF) {
        if (strcmp(command, "allocate") == 0) {
            if (strcmp(algorithm, "first_fit") == 0) {
                allocate_first_fit(adrs, size);

            } else if (strcmp(algorithm, "best_fit") == 0) {
                allocate_best_fit(adrs, size);
            } else if (strcmp(algorithm, "worst_fit") == 0) {
                allocate_worst_fit(adrs, size);
            }
            query_count++;
        } else if (strcmp(command, "clear") == 0) {
            clear(adrs);
            query_count++;
        } else if (strcmp(command, "end") == 0) {
            break;
        } else {
            printf("Invalid command!\n");
        }

    }
    double execution_time = elapsed_time();
    fclose(file);
    double throughput = (double)query_count / execution_time;

    printf("Throughput of %s (queries per second): %.2f\n", algorithm, throughput);
}

int main() {
    execute("first_fit");
    execute("best_fit");
    execute("worst_fit");
    return 0;

}