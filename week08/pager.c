#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define PAGE_SIZE 8

struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
};

int num_pages;
int num_frames;
struct PTE *page_table;
// RAM as an array of strings of size num_frames
// disk as an array of strings of size num_pages
char **RAM;
char **disk;


int disk_accesses = 0;

void evict_page() {
    // Choose a random frame as a victim page (for simplicity)
    int victim_frame = rand() % num_frames;

    // If the victim page is dirty, simulate writing to disk
    if (page_table[victim_frame].dirty) {
        disk_accesses++;
        memcpy(disk[victim_frame], RAM[victim_frame], PAGE_SIZE);
    }

    // Update the page table to indicate that the victim page is not in RAM
    page_table[victim_frame].valid = false;

    // Notify the MMU with SIGCONT that the page is loaded
    kill(page_table[victim_frame].referenced, SIGCONT);
}

void process_page_request() {

    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].referenced != 0) {
            int mmu_pid = page_table[i].referenced;
            // MMU wants this page
            printf("A disk access request from MMU Process (pid=%d)\n", page_table[i].referenced);
            printf("Page %d is referenced\n", i);
            if (page_table[i].valid) {
                printf("Page %d is in RAM\n", i);
                page_table[i].referenced = 0;
                kill(page_table[i].referenced, SIGCONT);
            } else if (num_frames > 0) {
                // There's a free frame; load the page into RAM
                for (int j = 0; j < num_frames; j++) {
                    if (!page_table[j].valid) {
                        // print We can allocate it to free frame 0
                        printf("We can allocate it to free frame %d\n", j);
                        printf("Copy data from disk (page=%d) to RAM (frame=%d)\n", i, j);
                        page_table[i].frame = j;
                        page_table[i].valid = true;
                        page_table[i].dirty = false;
                        page_table[i].referenced = 0;
                        memcpy(RAM[j], disk[i], PAGE_SIZE);
                        // print the RAM array
                        disk_accesses++;
                        printf("RAM array\n");
                        for (int k = 0; k < num_frames; k++) {
                            printf("Frame %d ---> %s\n", k, RAM[k]);
                        }
                        printf("disk accesses is %d so far\n", disk_accesses);
                        printf("Resume MMU process\n");
                        // print the mmu_pid
                        printf("mmu_pid is %d\n", mmu_pid);
                        kill(mmu_pid, SIGCONT);
                        break;
                    }
                }
            } else {
                // No free frames; perform page replacement (evict)
                evict_page();
                // Load the requested page into the newly freed frame
                int mmu_pid = page_table[i].referenced;
                page_table[i].frame = i;
                page_table[i].valid = true;
                page_table[i].dirty = false;
                page_table[i].referenced = 0;
                memcpy(RAM[i], disk[i], PAGE_SIZE);
                kill(mmu_pid, SIGCONT);
            }
        }
    }
}

void sigusr1_handler(int signo) {
    // use process_page_request() to handle the page request from MMU
    if (signo == SIGUSR1) {
        printf("Pager received SIGUSR1 signal from MMU\n");
        process_page_request();
    }

}

void sigcont_handler(int signo) {
    // Handle SIGCONT (page loaded)
    // Wake up from sleep to continue processing
    if (signo == SIGCONT) {
        printf("Pager received SIGCONT signal from MMU\n");
        (void) signo;
    }
}

void initialize_page_table() {
    // create the /tmp/ex2/pagetable file and mmap it
    int fd = open("/tmp/ex2/pagetable", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open /tmp/ex2/pagetable");
        exit(1);
    }
    // initialize the page table
    ftruncate(fd, num_pages * sizeof(struct PTE));
    page_table = mmap(NULL, num_pages * sizeof(struct PTE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (page_table == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }

    for (int i = 0; i < num_pages; i++) {
        page_table[i].valid = false;
        page_table[i].frame = -1;
        page_table[i].dirty = false;
        page_table[i].referenced = 0;
    }

    // print ---------------------------------
    printf("-------------------------\n");
    printf("Initialized page table\n");
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n",
               i, page_table[i].valid, page_table[i].frame, page_table[i].dirty, page_table[i].referenced);
    }

    close(fd);
}

void initialize_disk() {
    // Initialize the disk array with empty pages
    disk = (char **)malloc(num_pages * sizeof(char *));
    for (int i = 0; i < num_pages; i++) {
        disk[i] = (char *)malloc(PAGE_SIZE);
        switch (i) {
            case 0:
                strcpy(disk[i], "gEefwaq");
                break;
            case 1:
                strcpy(disk[i], "kjQ2eeq");
                break;
            case 2:
                strcpy(disk[i], "43R2e2e");
                break;
            case 3:
                strcpy(disk[i], "jji2u32");
                break;
            default:
                memset(disk[i], 0, PAGE_SIZE);
                break;
        }
    }

    printf("-------------------------\n");
    printf("Initialized disk\nDisk array\n");
    for (int i = 0; i < num_pages; i++) {
        printf("Disk %d ---> %s\n", i, disk[i]);
    }

}

void initialize_RAM() {
    // Initialize the RAM array with empty pages
    RAM = (char **)malloc(num_frames * sizeof(char *));
    for (int i = 0; i < num_frames; i++) {
        RAM[i] = (char *)malloc(PAGE_SIZE);
        memset(RAM[i], 0, PAGE_SIZE);
    }

    printf("-------------------------\n");
    printf("Initialized RAM\nRAM array\n");
    for (int i = 0; i < num_frames; i++) {
        printf("RAM %d ---> %s\n", i, RAM[i]);
    }
}



void cleanup() {
    // Clean up and unmap the page table
    for (int i = 0; i < num_pages; i++) {
        free(disk[i]);
    }
    free(disk);
    for (int i = 0; i < num_frames; i++) {
        free(RAM[i]);
    }
    free(RAM);
    munmap(page_table, num_pages * sizeof(struct PTE));
    printf("Total disk accesses: %d\n", disk_accesses);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s num_pages num_frames\n", argv[0]);
        return 1;
    }

    printf("Pager process, ID: %d\n", getpid());

    num_pages = atoi(argv[1]);
    num_frames = atoi(argv[2]);
    // Initialize the page table, disk, and RAM
    initialize_page_table();
    initialize_RAM();
    initialize_disk();


    // Set up signal handlers
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGCONT, sigcont_handler);

    // Wait for SIGUSR1 signal (MMU page request)
    pause();
//
//    // Process the page request and handle page loading/eviction
//    process_page_request();

    // Cleanup and exit
    cleanup();
}
