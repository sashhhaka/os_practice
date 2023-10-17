#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int pid;

void sigusr1(int signum) {
    const char* path_txt = "text.txt";
    char text[1024];

    FILE* txt = fopen(path_txt, "r");
    while (fgets(text, sizeof(text), txt) != NULL) {
        printf("%s\n", text);
    }
    fclose(txt);
}

void sigusr2(int signum) {
    printf("Process terminating...\n");
    remove("agent.pid");
    exit(0);
}

int main(){
    const char* path_pid = "agent.pid";
    const char* path_txt = "text.txt";
    char text[1024];

    FILE* file = fopen(path_pid, "w");
    pid = getpid();
    fprintf(file, "%d", pid);
    fclose(file);

    FILE* txt = fopen(path_txt, "r");
    while (fgets(text, sizeof(text), txt) != NULL) {
        printf("%s\n", text);
    }
    fclose(txt);

    signal(SIGUSR1, sigusr1);
    signal(SIGUSR2, sigusr2);

    while (1){
        sleep(1);
    }
}