#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

int pid_int;

void sign_exit(int signal) {
    if (signal == SIGINT) {
        kill(pid_int, SIGTERM);
        exit(0);
    }
}

int main() {
    const char* path_pid = "agent.pid";
    int file = open(path_pid, O_RDONLY);

    if(file < 0){
        printf("Error: No agent found\n");
        exit(0);
    }

    char pid[20];
    read(file, pid, sizeof(pid));
    close(file);

    pid_int = atoi(pid);

    char input[10];

    signal(SIGINT, sign_exit);

    while(1){
        printf("Choose a command {“read”, “exit”, “stop”, “continue”} to send to the agent\n");
        scanf("%s", input);

        if (strcmp(input, "read") == 0) {
            kill(pid_int, SIGUSR1);
        } else if (strcmp(input, "exit") == 0) {
            kill(pid_int, SIGUSR2);
            exit(0);
        } else if (strcmp(input, "stop") == 0) {
            kill(pid_int, SIGTSTP);
        } else if (strcmp(input, "continue") == 0) {
            kill(pid_int, SIGCONT);
        }
    }

    return 0;
}