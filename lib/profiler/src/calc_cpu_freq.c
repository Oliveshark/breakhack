#include <stdio.h>
#include <stdint.h>
#include <rdtsc.h>

typedef double f64;
typedef uint64_t u64;

int
main(void)
{
	u64 os_freq = get_os_time_freq();
	u64 cpu_start = read_cpu_timer();
	u64 os_start = read_os_timer();
	u64 os_elapsed = 0;
	u64 os_wait_time = os_freq;
	while (os_elapsed < os_wait_time) {
		u64 os_end = read_os_timer();
		os_elapsed = os_end - os_start;
	}

	u64 cpu_end = read_cpu_timer();
	u64 cpu_elapsed = cpu_end - cpu_start;
	u64 cpu_freq = 0;
	if (os_elapsed) {
		cpu_freq = (u64)((double)os_freq * cpu_elapsed / os_elapsed);
	}

	printf("    OS Freq: %lu (reported)\n", os_freq);
	printf(" OS Seconds: %.4f\n", (f64)os_elapsed / (f64)os_freq);
	printf("  CPU Timer: %lu -> %lu = %lu\n", cpu_start, cpu_end, cpu_elapsed);
	printf("   CPU Freq: %lu (guessed)\n", cpu_freq);
}
