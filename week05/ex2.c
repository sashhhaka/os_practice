#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_MESSAGE_LENGTH 256

struct Thread {
    pthread_t id;
    int i;
    char message[MAX_MESSAGE_LENGTH];
};

void* thread_function(void* arg) {
    struct Thread* thread_data = (struct Thread*)arg;
    printf("Thread %i prints message: %s\n", thread_data->i, thread_data->message);
    printf("My id is %lu\n", thread_data->id);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Invalid value of n: %s\n", argv[1]);
        return 1;
    }

    struct Thread threads[n];

    for (int i = 0; i < n; i++) {
        threads[i].i = i + 1;
        snprintf(threads[i].message, MAX_MESSAGE_LENGTH, "Hello from thread %i", threads[i].i);
        printf("Thread %i is created\n", threads[i].i);

        // Create the thread
        if (pthread_create(&threads[i].id, NULL, thread_function, &threads[i]) != 0) {
            perror("pthread_create");
            return 1;
        }

        // Wait for the thread to finish
        if (pthread_join(threads[i].id, NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }

    return 0;
}