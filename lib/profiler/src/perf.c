#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "perf.h"

static int _FD = 0;

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags)
{
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                  group_fd, flags);
    return ret;
}

void perf_setup(void)
{
    static struct perf_event_attr pe;

    if (_FD) {
	    close(_FD);
	    _FD = 0;
    }

    pe.type = PERF_TYPE_SOFTWARE;
    pe.size = sizeof(pe);
    pe.config = PERF_COUNT_SW_PAGE_FAULTS;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    _FD = perf_event_open(&pe, 0, -1, -1, 0);
    if (_FD == -1) {
        fprintf(stderr, "Error opening leader %llx : %s\n", pe.config, strerror(errno));
        fprintf(stderr, "Page errors will not be counted\n");
        _FD = 0;
        return;
    }

    ioctl(_FD, PERF_EVENT_IOC_RESET, 0);
    ioctl(_FD, PERF_EVENT_IOC_ENABLE);
}

void perf_reset_page_fault_count(void)
{
    if (_FD) {
        ioctl(_FD, PERF_EVENT_IOC_RESET, 0);
        ioctl(_FD, PERF_EVENT_IOC_ENABLE);
    }
}

uint64_t perf_read_page_fault_count(void)
{
    size_t count = 0;
    if (_FD) {
        ssize_t bytes = read(_FD, &count, sizeof(count));
	if (bytes != sizeof(count)) {
		count = 0;
	}
    }
    return count;
}

void perf_close(void)
{
    if (_FD) {
        close(_FD);
    }
}
