#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>

#define NUM_EVENTS 2

int main()
{
    struct perf_event_attr pe[NUM_EVENTS];
    int fd[NUM_EVENTS];
    uint64_t count[NUM_EVENTS];

    memset(&pe, 0, sizeof(struct perf_event_attr) * NUM_EVENTS);

    // Define the performance events to monitor
    pe[0].type = PERF_TYPE_HARDWARE;
    pe[0].config = PERF_COUNT_HW_INSTRUCTIONS;
    pe[1].type = PERF_TYPE_HARDWARE;
    pe[1].config = PERF_COUNT_HW_CPU_CYCLES;

    // Create the performance counters
    for (int i = 0; i < NUM_EVENTS; i++) {
        fd[i] = syscall(__NR_perf_event_open, &pe[i], 0, -1, -1, 0);
        if (fd[i] == -1) {
            printf("Error opening perf event %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Start the performance counters
    for (int i = 0; i < NUM_EVENTS; i++) {
        ioctl(fd[i], PERF_EVENT_IOC_RESET, 0);
        ioctl(fd[i], PERF_EVENT_IOC_ENABLE, 0);
    }

    // Run your code here...

    // Stop the performance counters and read the counts
    for (int i = 0; i < NUM_EVENTS; i++) {
        ioctl(fd[i], PERF_EVENT_IOC_DISABLE, 0);
        read(fd[i], &count[i], sizeof(uint64_t));
        close(fd[i]);
    }

    // Print the results
    printf("Instructions: %llu\n", count[0]);
    printf("CPU Cycles: %llu\n", count[1]);

    return 0;
}