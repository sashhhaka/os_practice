#include <stdio.h>

#define MAX_STRING_SIZE 256

int main() {
    char buffer[MAX_STRING_SIZE];
    char chr;
    int length = 0;

    printf("Enter a string character by character until dot (.) (maximum %d characters):\n", MAX_STRING_SIZE);

    while (1) {
        chr = getchar();

        if (chr == '.' || chr == '\n') {
            break;
        }

        if (length < MAX_STRING_SIZE - 1) {
            buffer[length++] = chr;
        }
    }

    buffer[length] = '\0';

    printf("\"");
    for (int i = length - 1; i >= 0; i--) {
        putchar(buffer[i]);
    }
    printf("\"\n");

    return 0;
}
