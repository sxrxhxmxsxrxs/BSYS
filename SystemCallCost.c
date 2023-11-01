#include <stdio.h>
#include <stdint.h> //for rdsc
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define ITERATIONS 100000000
// calc standard deviation of results with iterations = x
// high standard deviation -> lot of variability -> increase iterations
// low standard deviation -> decrease iterations allowed

/*int main() {
    struct timeval start, end;
    double total_time = 0.0;

    for (int i = 0; i < ITERATIONS; i++) {

        gettimeofday(&start, NULL);  // start time

        read(0, NULL, 0); // null system call

        gettimeofday(&end, NULL);    // end time

        // Use of .tv_usec (microsec) and .tv_sec (sec) for precise time representation
        // of both fraction and second, displayed as microseconds
        double time_in_microseconds = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);
        total_time += time_in_microseconds;
    }

    double avg_time = total_time / ITERATIONS;
    printf("Average time for the null system call: %f microseconds\n", avg_time);

    return 0;
}*/


//WITH low level instruction rdtsc

// Inline assembly code
static inline uint64_t rdtsc() { // TSC (time stamp counter) register to count CPU cylces
    unsigned int lo, hi; //where lower and upper 32 bits are stored
    __asm__ volatile /* to insert unoptimzed assembly into machinecode */("rdtsc" : "=a" (lo), "=d" (hi));//assembly instruction rdtsc; return values
    return ((uint64_t)hi << 32) | lo; // to make a single 64 bit
    // details on what happens in return: hi gets 32 bit shift to left (now in upper part of 64-bit)
    // bitwise OR with lo (now in lower part of 64-bit)
}

int main() {
    uint64_t start, end;
    double total_time = 0.0;

    for (int i = 0; i < ITERATIONS; i++) {

        start = rdtsc();  // start time

        read(0, NULL, 0); // null system call

        end = rdtsc();    // end time

        uint64_t cycles = end - start;

        double time_in_microseconds = (double)cycles / 1000; // Nano to micro seconds

        total_time += time_in_microseconds;
    }

    double cpu_frequency_ghz = 1.5; // Individual CPU clock speed in GHz ( find in task manager -> performance -> base speed )

    double avg_time = (total_time / ITERATIONS) / cpu_frequency_ghz;

    printf("Average time for the null system call: %f microseconds\n", avg_time);

    return 0;
}

