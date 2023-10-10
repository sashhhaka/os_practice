#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>

// Function to check if a number is prime
bool is_prime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

// Structure to hold a range for prime counting
typedef struct {
    int start;
    int end;
    int count;
} PrimeRange;

// Function for each thread to count primes in its assigned range
void *count_primes(void *arg) {
    PrimeRange *range = (PrimeRange *)arg;
    range->count = 0;

    for (int i = range->start; i < range->end; i++) {
        if (is_prime(i)) {
            range->count++;
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <n> <m>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    pthread_t threads[m];
    PrimeRange ranges[m];
    int total_count = 0;

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    // Divide the range [0, n) into equal subintervals for each thread
    int range_size = n / m;
    for (int i = 0; i < m; i++) {
        ranges[i].start = i * range_size;
        ranges[i].end = (i == m - 1) ? n : (i + 1) * range_size;
        pthread_create(&threads[i], NULL, count_primes, &ranges[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < m; i++) {
        pthread_join(threads[i], NULL);
        total_count += ranges[i].count;
    }

    gettimeofday(&end_time, NULL);

    // Calculate execution time
    double execution_time = (end_time.tv_sec - start_time.tv_sec) +
                            (end_time.tv_usec - start_time.tv_usec) / 1e6;

    printf("Number of prime numbers in [0, %d): %d\n", n, total_count);
    printf("Execution time: %.6f seconds\n", execution_time);

    return 0;
}