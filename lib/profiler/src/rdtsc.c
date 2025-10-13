#include <sys/time.h>
#include <stdint.h>
#define PROFILER
#include "rdtsc.h"

#ifdef __arm__
static uint64_t rdtsc(void)
{
    uint64_t val;
    asm volatile("mrs %0, cntvct_el0" : "=r" (val));
    return val;
}
#else
#include <x86intrin.h>
#define rdtsc() __rdtsc()
#endif

uint64_t get_os_time_freq(void)
{
    return 1000000;
}

uint64_t read_os_timer(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return get_os_time_freq()*tv.tv_sec + tv.tv_usec;
}

inline uint64_t read_cpu_timer(void)
{
    return rdtsc();
}

uint64_t estimate_cpu_freq(uint64_t test_time)
{
    uint64_t os_freq = get_os_time_freq();
    uint64_t cpu_start = read_cpu_timer();
    uint64_t os_start = read_os_timer();
    uint64_t os_elapsed = 0;
    uint64_t os_wait_time = os_freq * test_time / 1000;
    while (os_elapsed < os_wait_time) {
        uint64_t os_end = read_os_timer();
        os_elapsed = os_end - os_start;
    }

    uint64_t cpu_end = read_cpu_timer();
    uint64_t cpu_elapsed = cpu_end - cpu_start;
    uint64_t cpu_freq = 0;
    if (os_elapsed) {
        cpu_freq = os_freq * cpu_elapsed / os_elapsed;
    }

    return cpu_freq;
}
