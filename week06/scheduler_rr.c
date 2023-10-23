#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#define PS_MAX 10

int TIME_QUANTUM = 1;

// holds the scheduling data of one process
typedef struct{
    int idx; // process idx (index)
    int at, bt, rt, wt, ct, tat; // arrival time, burst time, response time, waiting time, completion time, turnaround time.
    int burst; // remaining burst (this should decrement when the process is being executed)
} ProcessData;

// the idx of the running process
int running_process = -1; // -1 means no running processes

// the total time of the timer
unsigned total_time; // should increment one second at a time by the scheduler

// data of the processes
ProcessData data[PS_MAX]; // array of process data
int completed[PS_MAX];

// array of all process pids
pid_t ps[PS_MAX]; // zero valued pids - means the process is terminated or not created yet

// size of data array
unsigned data_size;

void read_file(FILE* file) {
    // Initialize the data_size variable to zero.
    data_size = 0;
    char line[256];
    fgets(line, sizeof(line), file);

    // Loop to read data from the file and store it in the data array.
    while (fscanf(file, "%d %d %d", &data[data_size].idx, &data[data_size].at, &data[data_size].bt) == 3) {
        // Initialize other fields of the process data.
        data[data_size].rt = data[data_size].bt;
        data[data_size].wt = 0;
        data[data_size].ct = 0;
        data[data_size].tat = 0;
        data[data_size].burst = data[data_size].bt;

        // Increment data_size to keep track of the number of processes read.
        data_size++;

        // Check if the data_size exceeds the maximum limit (PS_MAX).
        if (data_size >= PS_MAX) {
            printf("Exceeded the maximum limit of processes (PS_MAX).\n");
            break;
        }
    }
}


// send signal SIGCONT to the worker process
void resume(pid_t process) {
    // send signal SIGCONT to the worker process if it is not in one of the states
    // (1.not created yet or 2.terminated)
    if (process == -1 || ps[process] == 0) {
        return; // Process is not created or is terminated.
    }
    kill(ps[process], SIGCONT);
}

// send signal SIGTSTP to the worker process
void suspend(pid_t process) {
    // send signal SIGTSTP to the worker process if it is not in one of the states
    // (1.not created yet or 2.terminated)
    if (process == -1 || ps[process] == 0) {
        return; // Process is not created or is terminated.
    }
    kill(ps[process], SIGTSTP);

}

// send signal SIGTERM to the worker process
void terminate(pid_t process) {
    // send signal SIGTERM to the worker process if it is not in one of the states
    // (1.not created yet or 2.terminated)
    if (process == -1 || ps[process] == 0) {
        return; // Process is not created or is terminated.
    }
    kill(ps[process], SIGTERM);
    waitpid(ps[process], NULL, 0);

    // Calculate metrics (ct, tat, wt) for the terminated process and update data array.
    data[process].ct = total_time;
    data[process].tat = data[process].ct - data[process].at;
    data[process].wt = data[process].tat - data[process].bt;

    // Reset process data.
    ps[process] = 0;
}

// create a process using fork
void create_process(int new_process) {

    // stop the running process
    if (running_process != -1) {
        suspend(running_process);
        printf("Scheduler: Stopping Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
    }

    // fork a new process and add it to ps array
    pid_t pid = fork();

    // Now the idx of the running process is new_process
    running_process = new_process;

    //  The scheduler process runs the program "./worker {new_process}"
    // using one of the exec functions like execvp
    if (pid == 0) {
        // This is the child process.
        char process_idx[10];
        sprintf(process_idx, "%d", new_process);
        char* args[] = {"./worker", process_idx, NULL};
        execvp(args[0], args);

        // If execvp fails, an error occurred.
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // This is the parent process.
        ps[new_process] = pid;
        running_process = new_process;
    } else {
        // Forking failed.
        perror("fork");
        exit(EXIT_FAILURE);
    }

}





// find next process for running
ProcessData find_next_process() {

    // location of next process in {data} array
    int location = 0;
    int mn = (int) 1e9;

    for (int i = 0; i < data_size; i++) {

        // find location of the next process to run from the {data} array
        // Considering the scheduling algorithm FCFS
        if (data[i].burst <= 0)continue;
        if (data[i].at < mn){
            location = i;
            mn = data[i].at;
        }
    }

    // if next_process did not arrive so far,
    // then we recursively call this function after incrementing total_time
    if (data[location].at > total_time) {

        printf("Scheduler: Runtime: %u seconds.\nProcess %d: has not arrived yet.\n", total_time, location);

        // increment the time
        total_time++;
        return find_next_process();
    }

    // return the data of next process
    return data[location];
}


// reports the metrics and simulation results
void report(){
    printf("Simulation results.....\n");
    int sum_wt = 0;
    int sum_tat = 0;
    for (int i=0; i< data_size; i++){
        printf("process %d: \n", i);
        printf("	at=%d\n", data[i].at);
        printf("	bt=%d\n", data[i].bt);
        printf("	ct=%d\n", data[i].ct);
        printf("	wt=%d\n", data[i].wt);
        printf("	tat=%d\n", data[i].tat);
        printf("	rt=%d\n", data[i].rt);
        sum_wt += data[i].wt;
        sum_tat += data[i].tat;
    }

    printf("data size = %d\n", data_size);
    float avg_wt = (float)sum_wt/data_size;
    float avg_tat = (float)sum_tat/data_size;
    printf("Average results for this run:\n");
    printf("	avg_wt=%f\n", avg_wt);
    printf("	avg_tat=%f\n", avg_tat);
}

void check_burst(){

    for(int i = 0; i < data_size; i++)
        if (data[i].burst > 0)
            return;

    // report simulation results
    report();

    // terminate the scheduler :)
    exit(EXIT_SUCCESS);
}


/// This function is called every one second as a handler for SIGALRM signal
void schedule_handler(int signum) {
    // Increment the total time
    total_time++;

    // 1. If there is a worker process running, decrement its remaining burst by the time quantum.
    if (running_process != -1) {
        int time_to_decrement = TIME_QUANTUM;
        if (data[running_process].burst < TIME_QUANTUM) {
            time_to_decrement = data[running_process].burst;
        }

        data[running_process].burst -= time_to_decrement;

        printf("Scheduler: Runtime: %u seconds.\nScheduler: Process %d is running with %d seconds left\n", total_time, running_process, data[running_process].burst);

        // 1.A. If the worker process hasn't finished its burst time, reschedule it.
        if (data[running_process].burst > 0) {
            return;
        }

        // 1.B. If the worker process has finished its burst time, terminate it.
        printf("Scheduler: Terminating Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);

        // Wait for its termination and calculate its metrics (ct, tat, wt).
        terminate(running_process);

        // Reset the running_process.
        completed[running_process] = 1; // mark the process as completed.
        running_process = -1;
    }

    // 2. After that, find the next process to run.
    check_burst();
    ProcessData next_process = find_next_process();
    if (next_process.idx == -1) {
        // No process is found.
        printf("Scheduler: Runtime: %u seconds.\nProcess has not arrived yet.\n", total_time);
        return;
    }

    // 3. Create or resume the process.
    create_process(next_process.idx);
    printf("Scheduler: Starting Process %d (Remaining Time: %d)\n", next_process.idx, data[next_process.idx].burst);

    // Calculate the metric rt for the new process.
    data[next_process.idx].rt = total_time - data[next_process.idx].at;

    // If the process's response time equals its burst time, resume it.
    if (data[next_process.idx].rt == data[next_process.idx].bt) {
        printf("Scheduler: Resuming Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
        resume(running_process);
    }
}



int main(int argc, char *argv[]) {
    total_time = 0;

    printf("Enter the time quantum for Round Robin scheduling: ");
    scanf("%d", &TIME_QUANTUM);



    // read the data file
    FILE *in_file  = fopen(argv[1], "r");
    if (in_file == NULL) {
        printf("File is not found or cannot open it!\n");
        exit(EXIT_FAILURE);
    } else {
        read_file(in_file);
    }

    // set a timer
    struct itimerval timer;

    // the timer goes off 1 second after reset
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    // timer increments 1 second at a time
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    // this counts down and sends SIGALRM to the scheduler process after expiration.
    setitimer(ITIMER_REAL, &timer, NULL);

    // register the handler for SIGALRM signal
    signal(SIGALRM, schedule_handler);

    // Wait till all processes finish
    while(1); // infinite loop
}
