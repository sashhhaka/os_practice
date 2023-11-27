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
void move_buffer(unsigned short* buffer) {
    for (int i = 0; i < BUFF_SIZE-1; i++) buffer[i] = buffer[i + 1];
}

void print_buffer(unsigned short* buffer) {
    for (int i = 0; i < BUFF_SIZE; i++) printf("%d ", buffer[i]);
    printf("\n");
}

void remove_key(unsigned short* buffer, unsigned short key) {
    // make the last instance of key in the buffer 0
    for (int i = BUFF_SIZE-1; i >= 0; i--) {
        if (buffer[i] == key) {
            buffer[i] = 0;
            break;
        }
    }
}

int main() {
    int fd = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // open file for writing the input into
    int fd_out = open("ex1.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd_out < 0) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    struct input_event ev;
    unsigned short buffer[BUFF_SIZE];
    unsigned short shortcut_terminate[2] = {KEY_E, KEY_X};
    unsigned short custom_shortcut[5] = {KEY_F, KEY_I, KEY_R, KEY_A, KEY_S};
    for (int i = 0; i < 6; i++) buffer[i] = 0;
    int i = 0;
    while (1) {
        i++;
        read(fd, &ev, sizeof(struct input_event));
        if (ev.type == EV_KEY && (ev.value == 0 || ev.value == 1 || ev.value == 2)) {
            char *type;
            if (ev.value == 0) type = "RELEASED";
            else if (ev.value == 1) type = "PRESSED";
            else type = "REPEATED";
            printf("%s 0x%04x (%d)\n", type, ev.code, ev.code);
            // also append to the file


            if (ev.value == 1) {
                // add the new key to the buffer
                move_buffer(buffer);
                buffer[BUFF_SIZE-1] = ev.code;
                // check if the buffer contains the shortcut
                // print buffer
//                print_buffer(buffer, 6);
//                print_buffer(shortcut_terminate, sizeof(shortcut_terminate)/sizeof(shortcut_terminate[0]));
//                print_buffer(custom_shortcut, sizeof(custom_shortcut)/sizeof(custom_shortcut[0]));
                if (is_substring(shortcut_terminate,
                                 sizeof(shortcut_terminate)/sizeof(shortcut_terminate[0]),buffer)) {
                    printf("Terminating\n");
                    write(fd_out, "Terminated\n", sizeof("Terminated\n"));
                    close(fd);
                    exit(EXIT_SUCCESS);
                }
                if (is_substring(custom_shortcut,
                                 sizeof(custom_shortcut)/sizeof(custom_shortcut[0]), buffer)) {
                    printf("Custom shortcut\n");
                    write(fd_out, "Custom shortcut\n", sizeof("Custom shortcut\n"));
                }
            } else if (ev.value == 0) {
                // remove the key from the buffer
                remove_key(buffer, ev.code);
            }

        }
    }
}

