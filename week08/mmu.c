#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>

#define PAGE_SIZE 8

struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
};

int pid_pager;
struct PTE *page_table;
int num_pages;
char *reference_string;

int disk_accesses = 0;


void process_memory_access(const char *access) {
    // Parse the memory access string (e.g., "R3" or "W7")
    char mode = access[0];
    int page = atoi(&access[1]);
    printf("-------------------------\n");

    if (mode == 'R') {
        printf("Read Request for page %d\n", page);
    } else if (mode == 'W') {
        printf("Write Request for page %d\n", page);
    } else {
        printf("Invalid mode\n");
        exit(1);
    }

    // Check if the page is in RAM (use page_table)
    if (!page_table[page].valid) {
        printf("It is not a valid page --> page fault\n");
        printf("Ask pager to load it from disk (SIGUSR1 signal) and wait\n");
        // Page is not in RAM, simulate a page fault
        page_table[page].referenced = getpid();
        kill(pid_pager, SIGCONT);  // Notify pager of page fault
        pause();  // Sleep until page is loaded (SIGCONT received)
    }

    printf("MMU resumed by SIGCONT signal from pager\n");
    // If it's a write access, set the dirty flag in the page table
    if (mode == 'W') {
        page_table[page].dirty = true;
    }
}

void sigusr1_handler(int signo) {
    // Handle SIGUSR1 (page fault)
    if (signo == SIGUSR1) {
        (void) signo;
    }

}

void sigcont_handler(int signo) {
    // Handle SIGCONT (page loaded)
    if (signo == SIGCONT) {
        (void) signo;
    }
}

void initialize_page_table(int num_pages) {
    // open the mapped file /tmp/ex2/pagetable
    int fd = open("/tmp/ex2/pagetable", O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    // map the file to memory
    ftruncate(fd, num_pages * sizeof(struct PTE));
    page_table = mmap(NULL, num_pages * sizeof(struct PTE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (page_table == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }

    printf("-------------------------\n");
    printf("Initialized page table\nPage table\n");
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n",
               i, page_table[i].valid, page_table[i].frame, page_table[i].dirty, page_table[i].referenced);
    }

    close(fd);
}


void cleanup() {
    // Clean up and unmap the mapped file
    // send sigusr1 to pager to terminate it
    printf("-------------------------\n");
    printf("Done all requests.\n");
    printf("MMU sends SIGUSR1 signal to the pager.\n");
    kill(pid_pager, SIGUSR1);
    printf("MMU terminates.\n");
    exit(0);
}


int main(int argc, char *argv[]) {

    struct sigaction sigact;

    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = sigcont_handler;
    sigaction(SIGCONT, &sigact, NULL);

    int num_pages = atoi(argv[1]);

    pid_pager = atoi(argv[3]);
    // parse the reference string
    reference_string = argv[2];

    // Initialize the page table
    initialize_page_table(num_pages);

    // Set up signal handlers
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGCONT, sigcont_handler);

    //   Process the reference string
    char *token = strtok(reference_string, " ");
    while (token != NULL) {
        process_memory_access(token);
        printf("Page table\n");
        for (int i = 0; i < num_pages; i++) {
            printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n",
                   i, page_table[i].valid, page_table[i].frame, page_table[i].dirty, page_table[i].referenced);
        }
        token = strtok(NULL, " ");
    }


    // Cleanup and exit
    cleanup();
}
