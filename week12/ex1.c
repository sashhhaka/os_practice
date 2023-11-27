#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>

#define BUFF_SIZE 6


// function to determine if a string is a substring of a second string
int is_substring(unsigned short str1[], int n, unsigned short str2[]) {
    // if the first string is a suffix in str2, it is a substring
    for (int i = 0; i < n; i++) {
        if (str1[n - i - 1] != str2[BUFF_SIZE - 1 - i]) return 0;
    }
    return 1;
}

// function to move the buffer one cell to the left
void move_buffer(unsigned short *buffer) {
    for (int i = 0; i < BUFF_SIZE - 1; i++) buffer[i] = buffer[i + 1];
}

void print_buffer(unsigned short *buffer) {
    for (int i = 0; i < BUFF_SIZE; i++) printf("%d ", buffer[i]);
    printf("\n");
}

// make the last instance of key in the buffer 0
void remove_key(unsigned short *buffer, unsigned short key) {
    for (int i = BUFF_SIZE - 1; i >= 0; i--) {
        if (buffer[i] == key) {
            buffer[i] = 0;
            break;
        }
    }
}

// custom shortcut function
void frog(FILE *fptr) {
    printf("  @..@ \n");
    printf(" (----)\n");
    printf("( >__< )\n");
    printf("^^ ~~ ^^ \n");
    fprintf(fptr, "  @..@ \n");
    fprintf(fptr, " (----)\n");
    fprintf(fptr, "( >__< )\n");
    fprintf(fptr, "^^ ~~ ^^ \n");

}

int main() {
    int fd = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // open file for writing the input into
    FILE *fptr = fopen("ex1.txt", "w");
    if (fptr == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    system("chmod 777 ex1.txt");

    struct input_event ev;

    // initialize the buffer
    unsigned short buffer[BUFF_SIZE];
    for (int i = 0; i < 6; i++) buffer[i] = 0;
    // initialize shortcuts
    unsigned short shortcut_terminate[2] = {KEY_E, KEY_X};
    unsigned short passed_shortcut[2] = {KEY_P, KEY_E};
    unsigned short shortcut_cap[3] = {KEY_C, KEY_A, KEY_P};
    unsigned short custom_shortcut[4] = {KEY_F, KEY_R, KEY_O, KEY_G};

    while (1) {
        read(fd, &ev, sizeof(struct input_event));
        if (ev.type == EV_KEY && (ev.value == 0 || ev.value == 1 || ev.value == 2)) {
            char *type;
            if (ev.value == 0) type = "RELEASED";
            else if (ev.value == 1) type = "PRESSED";
            else type = "REPEATED";
            printf("%s 0x%04x (%d)\n", type, ev.code, ev.code);

            fprintf(fptr, "%s 0x%04x (%d)\n", type, ev.code, ev.code);


            if (ev.value == 1) {
                // add the new key to the buffer
                move_buffer(buffer);
                buffer[BUFF_SIZE - 1] = ev.code;
                // check if the buffer contains the shortcut
                if (is_substring(shortcut_terminate,
                                 sizeof(shortcut_terminate) / sizeof(shortcut_terminate[0]), buffer)) {
                    printf("Terminating\n");
                    fprintf(fptr, "Terminating\n");
                    close(fd);
                    exit(EXIT_SUCCESS);
                }

                if (is_substring(passed_shortcut,
                                 sizeof(passed_shortcut) / sizeof(passed_shortcut[0]), buffer)) {
                    printf("I passed the Exam!\n");
                    fprintf(fptr, "I passed the Exam!\n");
                }

                if (is_substring(shortcut_cap,
                                 sizeof(shortcut_cap) / sizeof(shortcut_cap[0]), buffer)) {
                    printf("Get some cappuccino!\n");
                    fprintf(fptr, "Get some cappuccino!\n");
                }
                if (is_substring(custom_shortcut,
                                 sizeof(custom_shortcut) / sizeof(custom_shortcut[0]), buffer)) {
                    frog(fptr);

                }

            } else if (ev.value == 0) {
                // remove the last instance of key from the buffer
                remove_key(buffer, ev.code);
            }
        }
    }
}

