#ifndef _PERF_H_
#define _PERF_H_

#include <stdint.h>

void perf_setup(void);

void perf_reset_page_fault_count(void);

uint64_t perf_read_page_fault_count(void);

void perf_close(void);

#endif  // _PERF_H_

