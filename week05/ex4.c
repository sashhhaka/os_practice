#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

// Function to check if a number is prime
bool is_prime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

// The mutex
pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;

int k = 0;
int c = 0;

int n = 0;
int m = 0;

// Get the next prime candidate
int get_number_to_check() {
    int ret;
    pthread_mutex_lock(&global_lock);
    ret = k;
    if (k != n) {
        k++;
    }
    pthread_mutex_unlock(&global_lock);
    return ret;
}

// Increase prime counter
void increment_primes() {
    pthread_mutex_lock(&global_lock);
    c++;
    pthread_mutex_unlock(&global_lock);
}

void *check_primes(void *arg) {
    while (1) {
        int num_to_check = get_number_to_check();
        if (num_to_check >= n) {
            break; // All numbers have been checked
        }
        if (is_prime(num_to_check)) {
            increment_primes();
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <range_n> <number_of_threads>\n", argv[0]);
        return 1;
    }

    n = atoi(argv[1]);
    m = atoi(argv[2]);

    pthread_t threads[m];
    int thread_ids[m];

    for (int i = 0; i < m; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, check_primes, &thread_ids[i]);
    }

    for (int i = 0; i < m; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Total prime numbers found: %d\n", c);

    return 0;
}
