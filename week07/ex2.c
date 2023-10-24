#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ctype.h>

#define MAX_FILE_SIZE (500 * 1024 * 1024)
#define BUFFER_SIZE (500 * 1024)

int main() {
    FILE *file = fopen("text.txt", "w+");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_SET);


    long page_size = sysconf(_SC_PAGESIZE);
    long chunk_size = 1024 * page_size;

    if (ftruncate(fileno(file), MAX_FILE_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    char *addr = mmap(NULL, MAX_FILE_SIZE, PROT_WRITE, MAP_SHARED, fileno(file), 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    int random_fd = open("/dev/random", O_RDONLY);
    if (random_fd == -1) {
        perror("open");
        munmap(addr, MAX_FILE_SIZE);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    int buffer_index = 0;
    for (int i = 0; i < MAX_FILE_SIZE; i++) {
        if (buffer_index == 0) {
            ssize_t bytes_read = read(random_fd, buffer, BUFFER_SIZE);
            if (bytes_read == -1) {
                perror("read");
                munmap(addr, MAX_FILE_SIZE);
                exit(1);
            }
            buffer_index = bytes_read;
        }

        char c = buffer[--buffer_index];
        if (isprint(c)) {
            addr[i] = c;
        } else {
            i--;
        }

        if (i % 1024 == 0) {
            addr[i] = '\n';
        }
    }

    int capital_count = 0;
    for (off_t offset = 0; offset < MAX_FILE_SIZE; offset += chunk_size) {
        off_t size = chunk_size;
        if (offset + size > MAX_FILE_SIZE) {
            size = MAX_FILE_SIZE - offset;
        }

        addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(file), offset);
        if (addr == MAP_FAILED) {
            perror("mmap");
            exit(1);
        }

        for (off_t i = 0; i < size; i++) {
            char c = addr[i];
            if (isupper(c)) {
                capital_count++;
                addr[i] = tolower(c);
            }
        }

        if (munmap(addr, size) == -1) {
            perror("munmap");
            exit(1);
        }
    }

    printf("Total capital letters: %d\n", capital_count);

    close(random_fd);
    fclose(file);

    return 0;
}
