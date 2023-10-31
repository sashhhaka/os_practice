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
// reference string
char *reference_string;

int disk_accesses = 0;



void process_memory_access(const char *access) {
    // Parse the memory access string (e.g., "R3" or "W7")
    char mode = access[0];
    int page = atoi(&access[1]);
    // print Page 0 is referenced
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
        kill(pid_pager, SIGUSR1);  // Notify pager of page fault
        printf("MMU paused by SIGUSR1 signal from pager\n");
        pause();  // Sleep until page is loaded (SIGCONT received)
    }

    printf("MMU resumed by SIGCONT signal from pager\n");
    // If it's a write access, set the dirty flag in the page table
    if (mode == 'W') {
        page_table[page].dirty = true;
    }

    // Print the updated page table
    printf("Page Table %s:\n", access);
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d: Valid=%d, Frame=%d, Dirty=%d, Referenced=%d\n",
               i, page_table[i].valid, page_table[i].frame, page_table[i].dirty, page_table[i].referenced);
    }
    printf("\n");
}

void sigusr1_handler(int signo) {
    // Handle SIGUSR1 (page fault)
    if (signo == SIGUSR1) {
        printf("SIGUSR1 signal received\n");
        (void) signo;
    }

}


void sigcont_handler(int signo) {
    // get the token by the disk_accesses as an index
    // for example if disk_accesses is 0, then get the first token, if disk_accesses is 1, then get the second token
    if (signo == SIGCONT) {
        printf("SIGCONT signal received\n");
        char *token = &reference_string[disk_accesses*3];
        disk_accesses++;
        process_memory_access(token);
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



}



void cleanup() {
    // Clean up and unmap the mapped file
    munmap(page_table, num_pages * sizeof(struct PTE));
    exit(0);
}

void catcher() {}

int main(int argc, char *argv[]) {

    struct sigaction sigact;

    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = sigcont_handler;
    sigaction(SIGCONT, &sigact, NULL);

    if (argc != 4) {
        fprintf(stderr, "Usage: %s num_pages reference_string pager_pid\n", argv[0]);
        return 1;
    }


    int num_pages = atoi(argv[1]);

    pid_pager = atoi(argv[3]);
    // parse the reference string
    reference_string = argv[2];

    // print A disk access request from MMU Process (pid=283032)
    printf("-------------------------\n");
    printf("A disk access request from MMU Process (pid=%d)\n", getpid());

    // Initialize the page table
    initialize_page_table(num_pages);

    // Set up signal handlers
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGCONT, sigcont_handler);

    //   Process the reference string
    sigcont_handler(SIGCONT);

    pause();

    // Cleanup and exit
    cleanup();
}
