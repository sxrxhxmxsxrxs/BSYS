#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sched.h>
#include <pthread.h>
// compile with -lpthread !!!

#define ITERATIONS 1000000

int main() {
    int pipe_fd[2];
    struct timeval start, end;
    double time;

    cpu_set_t set; // If more than one physical CPU
    CPU_ZERO(&set);
    CPU_SET(0, &set);

    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed"); // Error Handliing
        exit(EXIT_FAILURE);
    }

    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // Child
        if(sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1){
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[1]);  // Close write
        char buffer[1];
        for (int i = 0; i < ITERATIONS; i++) {
            read(pipe_fd[0], buffer, 1);
            write(pipe_fd[0], "1", 1);
        }
        close(pipe_fd[0]);  // Close read
    } else {
        // Parent
        if(sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1){ // wenn man mehr als eine physische cpu hat
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[0]);  // Close read
        gettimeofday(&start, NULL);

        char buffer[1];
        for (int i = 0; i < ITERATIONS; i++) {
            write(pipe_fd[1], "1", 1);
            read(pipe_fd[1], buffer, 1);
        }
        close(pipe_fd[1]);  // Close write

        gettimeofday(&end, NULL);

        // Use of .tv_usec (microsec) and .tv_sec (sec) for precise time representation
        // of both fraction and second, displayed as microseconds
        time = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);

        printf("Average time for context switches: %f micro seconds\n", ( time / ITERATIONS) / 2); // Divide by two because there is a double context switch happening above!
    }

    return 0;
}
