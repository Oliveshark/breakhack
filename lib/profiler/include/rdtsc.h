#ifndef _RDTSC_H_
#define _RDTSC_H_

#ifdef PROFILER

#include <stdint.h>
#include <stddef.h>

uint64_t get_os_time_freq(void);

uint64_t read_os_timer(void);

uint64_t read_cpu_timer(void);

uint64_t estimate_cpu_freq(uint64_t test_time);

#endif

#endif // _RDTSC_H_
