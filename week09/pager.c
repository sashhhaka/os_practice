#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#define PAGE_SIZE 8

struct PageTableEntry {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
    int counter;
};

char **memory;
char **disk;
int *frames;

int number_of_frames;
int number_of_pages;
struct PageTableEntry *page_table;

int (*page_replacement_algorithm)(struct PageTableEntry *page_table);

int accesses_count = 0;
int evict_count = 0;

int my_random(struct PageTableEntry *page_table) {
    int victim_frame = rand() % number_of_frames;
    int page_to_evict = -1;
    for (int i = 0; i < number_of_pages; i++) {
        if (page_table[i].frame != victim_frame) continue;
        page_to_evict = i;
        break;
    }
    return page_to_evict;
}

int nfu(struct PageTableEntry *page_table) {
    int min_referenced = INT_MAX;
    int page_to_evict = -1;

    for (int i = 0; i < number_of_pages; i++) {
        if (page_table[i].valid && page_table[i].counter < min_referenced) {
            min_referenced = page_table[i].counter;
            page_to_evict = i;
        }
    }
    page_table[page_to_evict].counter++;

    return page_to_evict;
}

int aging(struct PageTableEntry *page_table) {
    int min_aging = INT_MAX;
    int page_to_evict = -1;

    for (int i = 0; i < number_of_pages; i++) {
        if (page_table[i].valid) {
            page_table[i].counter >>= 1;

            if (page_table[i].dirty) {
                page_table[i].counter |= 128;
            }

            if (page_table[i].counter < min_aging) {
                min_aging = page_table[i].counter;
                page_to_evict = i;
            }
        }
    }
    page_table[page_to_evict].counter++;

    return page_to_evict;
}

void evict_page(int page, int mmu_pid, int (*algorithm)(struct PageTableEntry *page_table)) {
    int page_to_evict = algorithm(page_table);
    int victim_frame = page_table[page_to_evict].frame;
    evict_count++;
    accesses_count++;

    printf("Victim frame %d was chosen\n", victim_frame);
    printf("Replace/Evict it with page %d to be allocated to frame %d\n", page, victim_frame);
    printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", page, victim_frame);

    if (page_table[page_to_evict].dirty) {
        memcpy(disk[page_to_evict], memory[victim_frame], PAGE_SIZE);
        accesses_count++;
    }

    memcpy(memory[victim_frame], disk[page], PAGE_SIZE);

    page_table[page_to_evict].frame = -1;
    page_table[page_to_evict].valid = false;
    page_table[page_to_evict].dirty = false;
    page_table[page_to_evict].referenced = 0;

    page_table[page].frame = victim_frame;
    page_table[page].valid = true;
    page_table[page].dirty = false;
    page_table[page].referenced = 0;

    printf("RAM array\n");
    for (int i = 0; i < number_of_frames; i++)
        printf("Frame %d ---> %s\n", i, memory[i]);

    printf("disk accesses is %d so far\n", accesses_count);
    printf("Resume MMU process\n");

    kill(mmu_pid, SIGCONT);
}

void request(int (*algorithm)(struct PageTableEntry *page_table)) {
    int free = 0;
    for (int i = 0; i < number_of_pages; i++) {
        if (page_table[i].referenced != 0) {
            int mmu_pid = page_table[i].referenced;
            printf("-------------------------\n");
            printf("A disk access request from MMU Process (pid=%d)\n", page_table[i].referenced);
            printf("Page %d is referenced\n", i);
            if (!page_table[i].valid) {
                if (number_of_frames <= 0) continue;
                for (int j = 0; j < number_of_frames; j++) {
                    if (!frames[j]) {
                        free++;
                        printf("We can allocate it to frame %d\n", j);
                        printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", i, j);
                        memcpy(memory[j], disk[i], PAGE_SIZE);
                        frames[j] = 1;

                        page_table[i].frame = j;
                        page_table[i].valid = true;
                        page_table[i].dirty = false;
                        page_table[i].referenced = 0;

                        printf("RAM array\n");

                        for (int i1 = 0; i1 < number_of_frames; i1++)
                            printf("Frame %d ---> %s\n", i1, memory[i1]);

                        accesses_count++;

                        printf("disk accesses is %d so far\n", accesses_count);
                        printf("Resume MMU process\n");

                        kill(mmu_pid, SIGCONT);
                        break;
                    }
                }

                if (!free) {
                    printf("We do not have free frames in RAM\n");
                    evict_page(i, mmu_pid, algorithm);
                    break;
                }
            } else {
                printf("Page %d is in RAM\n", i);
                page_table[i].referenced = 0;
                kill(page_table[i].referenced, SIGCONT);
            }
        }
    }
}

void sigcont_handler(int signo) {
    if (signo == SIGCONT) {
        request(page_replacement_algorithm);
    }
}

void sigusr1_handler(int signo) {
    if (signo == SIGUSR1) {
        printf("Pager received SIGCONT signal from MMU\n");
        printf("-------------------------\n");
        printf("%d disk accesses in total\n", accesses_count);
        printf("Pager is terminated\n");

        exit(0);
    }
}

void initialize_page_table() {
    int fd = open("/tmp/ex2/pagetable", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open /tmp/ex2/pagetable");
        exit(EXIT_FAILURE);
    }

    ftruncate(fd, number_of_pages * sizeof(struct PageTableEntry));
    page_table = mmap(NULL, number_of_pages * sizeof(struct PageTableEntry), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (page_table == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < number_of_pages; i++) {
        page_table[i].valid = false;
        page_table[i].frame = -1;
        page_table[i].dirty = false;
        page_table[i].referenced = 0;
    }

    printf("-------------------------\n");
    printf("Initialized page table\n");
    for (int i = 0; i < number_of_pages; i++)
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n",
               i, page_table[i].valid, page_table[i].frame, page_table[i].dirty, page_table[i].referenced);

    close(fd);
}

void initialize_disk() {
    disk = (char **)malloc(number_of_pages * sizeof(char *));
    static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    for (int i = 0; i < number_of_pages; i++) {
        disk[i] = (char *)malloc(PAGE_SIZE);

        for (int j = 0; j < PAGE_SIZE - 1; j++) {
            int index = rand() % (sizeof(charset) - 1);
            disk[i][j] = charset[index];
        }

        disk[i][PAGE_SIZE - 1] = '\0';
    }

    printf("-------------------------\n");
    printf("Initialized disk\nDisk array\n");
    for (int i = 0; i < number_of_pages; i++)
        printf("Disk %d ---> %s\n", i, disk[i]);
}

void initialize_memory() {
    memory = (char **)malloc(number_of_frames * sizeof(char *));
    frames = (int *)malloc(number_of_frames * sizeof(int));

    for (int i = 0; i < number_of_frames; i++) {
        memory[i] = (char *)malloc(PAGE_SIZE);
        memset(memory[i], 0, PAGE_SIZE);
        frames[i] = 0;
    }

    printf("-------------------------\n");
    printf("Initialized RAM\nRAM array\n");
    for (int i = 0; i < number_of_frames; i++)
        printf("RAM %d ---> %s\n", i, memory[i]);
}

void cleanup() {
    for (int i = 0; i < number_of_pages; i++) free(disk[i]);
    free(disk);
    for (int i = 0; i < number_of_frames; i++) free(memory[i]);
    free(memory);
    munmap(page_table, number_of_pages * sizeof(struct PageTableEntry));

    printf("Total disk accesses: %d\n", accesses_count);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s number_of_pages number_of_frames algorithm(random/nfu/aging)\n", argv[0]);
        return 1;
    }

    printf("Pager process, ID: %d\n", getpid());

    number_of_pages = atoi(argv[1]);
    number_of_frames = atoi(argv[2]);

    if (strcmp(argv[3], "random") == 0) {
        page_replacement_algorithm = my_random;
        printf("Selected page replacement algorithm: Random\n");
    } else if (strcmp(argv[3], "nfu") == 0) {
        page_replacement_algorithm = nfu;
        printf("Selected page replacement algorithm: NFU\n");
    } else if (strcmp(argv[3], "aging") == 0) {
        page_replacement_algorithm = aging;
        printf("Selected page replacement algorithm: Aging\n");
    }

    initialize_page_table();
    initialize_memory();
    initialize_disk();

    signal(SIGUSR1, sigusr1_handler);
    signal(SIGCONT, sigcont_handler);

    while (1) {
        pause();
    }
}
