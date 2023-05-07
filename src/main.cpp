#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <cstdint>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>

#define NUM_EVENTS 2

uint64_t array_size = 10;
uint64_t array[10] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9
};

void victim_function(uint64_t x) {
    if (x < array_size) {
        uint64_t temp = array[x];
    }
}

void observe_victim(uint64_t x) {
    struct perf_event_attr pe[NUM_EVENTS];
    int fd[NUM_EVENTS];
    uint64_t count[NUM_EVENTS];

    memset(&pe, 0, sizeof(struct perf_event_attr) * NUM_EVENTS);

    // Define the performance events to monitor
    pe[0].type = PERF_TYPE_HARDWARE;
    pe[0].config = PERF_COUNT_HW_CACHE_REFERENCES;
    pe[1].type = PERF_TYPE_HARDWARE;
    pe[1].config = PERF_COUNT_HW_CACHE_MISSES;

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
    std::cout << "Accessing index: " << x << std::endl;
    std::cout << "Cache references: " << count[0] << std::endl;
    std::cout << "Cache misses: " << count[1] << std::endl;
    std::cout << "Cache miss rate: " << count[1] / count[0] << std::endl;
}

int main()
{
    observe_victim(0);
    observe_victim(0);
    observe_victim(0);
    observe_victim(0);
    observe_victim(0);
    observe_victim(0);
    observe_victim(12);

    return 0;
}