#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

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
char **RAM;
char **disk;
int *frames;

//int mmu_pid;


int disk_accesses = 0;
int count_evict = 0;

void evict_page(int page, int mmu_pid) {
    int victim_frame = rand() % num_frames;
//    int victim_frame = count_evict % 2;

    count_evict++;
    disk_accesses++;


    printf("Choose a random victim page %d\n", victim_frame);

    printf("Replace/Evict it with page %d to be allocated to frame %d\n", page, victim_frame);
    printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", page, victim_frame);

    // find page in pagetable with frame = victim_frame
    int page_to_evict = -1;
    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].frame == victim_frame) {
            page_to_evict = i;
            break;
        }
    }

    if (page_table[page_to_evict].dirty) {
        disk_accesses++;
        memcpy(disk[page_to_evict], RAM[victim_frame], PAGE_SIZE);
    }

    memcpy(RAM[victim_frame], disk[page], PAGE_SIZE);

    page_table[page_to_evict].frame = -1;
    page_table[page_to_evict].valid = false;
    page_table[page_to_evict].dirty = false;
    page_table[page_to_evict].referenced = 0;

    page_table[page].frame = victim_frame;
    page_table[page].valid = true;
    page_table[page].dirty = false;
    page_table[page].referenced = 0;

    // print RAM array
    printf("RAM array\n");
    for (int i = 0; i < num_frames; i++) {
        printf("Frame %d ---> %s\n", i, RAM[i]);
    }

    printf("disk accesses is %d so far\n", disk_accesses);
    printf("Resume MMU process\n");

    kill(mmu_pid, SIGCONT);

}

void process_page_request() {
    int free_frame = 0;

    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].referenced != 0) {
            int mmu_pid = page_table[i].referenced;
            // MMU wants this page
            printf("-------------------------\n");
            printf("A disk access request from MMU Process (pid=%d)\n", page_table[i].referenced);
            printf("Page %d is referenced\n", i);
            if (page_table[i].valid) {
                printf("Page %d is in RAM\n", i);
                page_table[i].referenced = 0;
                kill(page_table[i].referenced, SIGCONT);
            } else if (num_frames > 0) {
                // There's a free frame; load the page into RAM
                for (int j = 0; j < num_frames; j++) {
                    // count the number of free frames
                    // RAM[j] is not NULL
                    if (!frames[j]) {
                        free_frame++;
                        int free_frame_index = j;
                        printf("We can allocate it to frame %d\n", free_frame_index);
                        printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", i, free_frame_index);
                        memcpy(RAM[free_frame_index], disk[i], PAGE_SIZE);
                        frames[free_frame_index] = 1;
                        page_table[i].frame = free_frame_index;
                        page_table[i].valid = true;
                        page_table[i].dirty = false;
                        page_table[i].referenced = 0;
                        printf("RAM array\n");
                        for (int i = 0; i < num_frames; i++) {
                            printf("Frame %d ---> %s\n", i, RAM[i]);
                        }
                        disk_accesses++;
                        printf("disk accesses is %d so far\n", disk_accesses);
                        printf("Resume MMU process\n");
                        kill(mmu_pid, SIGCONT);
                        break;
                    }
                }
                if (!free_frame) {
                    printf("We do not have free frames in RAM\n");
                    evict_page(i, mmu_pid);
                    break;
                }
            }
        }
    }
}

void sigcont_handler(int signo) {
    // use process_page_request() to handle the page request from MMU
    if (signo == SIGCONT) {
        process_page_request();
    }

}

void sigusr1_handler(int signo) {
    // Handle SIGCONT (page loaded)
    // Wake up from sleep to continue processing
    if (signo == SIGUSR1) {
        printf("Pager received SIGCONT signal from MMU\n");
        printf("-------------------------\n");
        printf("%d disk accesses in total\n", disk_accesses);
        printf("Pager is terminated\n");
        exit(0);
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
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char **word;
    for (int i = 0; i < num_pages; i++) {
        disk[i] = (char *)malloc(PAGE_SIZE);
        if (i < 4) {
            // Initialize specific pages
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
            }
        } else {
            // Generate random words for other pages
            for (int j = 0; j < PAGE_SIZE - 1; j++) {
                int index = rand() % (sizeof(charset) - 1);
                disk[i][j] = charset[index];
            }
            disk[i][PAGE_SIZE - 1] = '\0'; // Null-terminate the string
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
    // set values in RAM to NULL
    RAM = (char **) malloc(num_frames * sizeof(char *));
    for (int i = 0; i < num_frames; i++) {
        RAM[i] = (char *) malloc(PAGE_SIZE);
        memset(RAM[i], 0, PAGE_SIZE);
    }
    // initialize frames to 0
    frames = (int *) malloc(num_frames * sizeof(int));
    for (int i = 0; i < num_frames; i++) {
        frames[i] = 0;
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

    // Wait for signals from MMU
    while (1) {
        pause();
    }


}
