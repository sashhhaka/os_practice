#include <stdio.h>

#define MAX_STRING_SIZE 256

void count(char str[], char c) {
    int i = 0;
    int count = 0;
    while (str[i] != '\0') {
        if (str[i] == c || str[i] == c + 32 || str[i] == c - 32) {
            count++;
        }
        i++;
    }
    if (c >= 'A' && c <= 'Z') {
        c += 32;
    }
    printf("%c:%d", c, count);

}

void countAll(char str[]) {
    int i = 0;
    while (str[i] != '\0') {
        count(str, str[i]);
        printf(", ");
        i++;
    }
}


int main() {
    char str[MAX_STRING_SIZE];
    scanf("%s", str);
    countAll(str);
    return 0;
}
