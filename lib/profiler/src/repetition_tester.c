#include <stdio.h>
#include <unistd.h>
#include "repetition_tester.h"
#include "internal/common.h"

#define PROFILER
#include "rdtsc.h"
#include "perf.h"

void
rept_setup(RepetitionTester *tester, uint64_t target_bytes, uint64_t cpu_timer_freq, uint32_t test_time_seconds)
{
	perf_setup();

	if (tester->mode == TestMode_Uninitialized) {
		tester->mode = TestMode_Testing;
		tester->target_processed_byte_count = target_bytes;
		tester->cpu_timer_freq = cpu_timer_freq;
		tester->print_new_mins = true;
		tester->results.min.E[RepVal_cpu_time] = (uint64_t)-1;
		tester->results.max.E[RepVal_cpu_time] = 0;
	} else if (tester->mode == TestMode_Completed) {
		tester->mode = TestMode_Testing;

		if (tester->target_processed_byte_count != target_bytes) {
			rept_error(tester, "target_processed_byte_count changed");
		}

		if (tester->cpu_timer_freq != cpu_timer_freq) {
			rept_error(tester, "CPU frequencey changed");
		}
	}

	tester->test_time = test_time_seconds * cpu_timer_freq;
	tester->start_time = read_cpu_timer();
}

void
rept_begin(RepetitionTester *tester)
{
	++tester->open_block_count;
	perf_reset_page_fault_count();

	RepetitionValue *curr = &tester->current;
	curr->E[RepVal_cpu_time] -= read_cpu_timer();
}

void
rept_end(RepetitionTester *tester)
{
	++tester->close_block_count;

	RepetitionValue *curr = &tester->current;
	curr->E[RepVal_cpu_time] += read_cpu_timer();
	curr->E[RepVal_page_faults] += perf_read_page_fault_count();
}

void
rept_count_bytes(RepetitionTester *tester, uint64_t byte_count)
{
	RepetitionValue *curr = &tester->current;
	curr->E[RepVal_byte_count] += byte_count;
}

static void
print_row(FILE *fp, const char *label, RepetitionValue val, uint64_t rate)
{
	static const double gb = 1024.0 * 1024.0 * 1024.0;
	uint64_t test_count = val.E[RepVal_test_count];
	double divisor = test_count ? (double)test_count : 1;

	double E[RepVal_count];
	for (size_t i = 0; i < RepVal_count; ++i) {
		E[i] = (double)val.E[i] / divisor;
	}

	uint64_t time = E[RepVal_cpu_time];
	double seconds = (double)time / rate;
	double millis = seconds * 1000.0;
	double gb_per_second = E[RepVal_byte_count] / (gb * seconds);

	if (isatty(fileno(fp))) {
		fprintf(fp, C_WHITE "%s:" C_GREEN " %lu " C_YELLOW "(%fms) " C_CYAN "%fgb/s" C_RESET, label, time,
		        millis, gb_per_second);
	} else {
		fprintf(fp, "%s: %lu (%fms) %fgb/s", label, time, millis, gb_per_second);
	}

	if (E[RepVal_page_faults] > 0) {
		if (isatty(fileno(fp))) {
			fprintf(fp, C_RED " PF: %0.4f (%0.4fk/fault)" C_RESET, E[RepVal_page_faults],
			        E[RepVal_byte_count] / (E[RepVal_page_faults] * 1024.0));
		} else {
			fprintf(fp, " PF: %.4f (%.4fk/fault)", E[RepVal_page_faults],
			        E[RepVal_byte_count] / (E[RepVal_page_faults] * 1024.0));
		}
	}
	fprintf(fp, "           \r");
}

bool
rept_is_testing(RepetitionTester *tester)
{
	if (tester->mode == TestMode_Testing) {
		RepetitionValue curr = tester->current;
		uint64_t curr_time = read_cpu_timer();

		if (tester->open_block_count) {

			if (tester->open_block_count != tester->close_block_count) {
				rept_error(tester, "Unbalanced begin/end time");
			} else if (curr.E[RepVal_byte_count] != tester->target_processed_byte_count) {
				rept_error(tester, "Processed byte count mismatch");
			} else {
				RepetitionTestResult *result = &tester->results;

				curr.E[RepVal_test_count] = 1;

				for (size_t i = 0; i < RepVal_count; ++i) {
					result->total.E[i] += curr.E[i];
				}
				if (result->max.E[RepVal_cpu_time] < curr.E[RepVal_cpu_time]) {
					result->max = curr;
				}
				if (result->min.E[RepVal_cpu_time] > curr.E[RepVal_cpu_time]) {
					result->min = curr;
					tester->start_time = curr_time;
					if (tester->print_new_mins) {
						print_row(stdout, "Min", result->min, tester->cpu_timer_freq);
						fflush(stdout);
					}
				}

				tester->open_block_count = 0;
				tester->close_block_count = 0;
				tester->current = (RepetitionValue){0};
			}
		}

		if (curr_time - tester->start_time > tester->test_time) {
			tester->mode = TestMode_Completed;
			perf_close();
			rept_print_results(tester, stdout);
		}
	}
	return tester->mode == TestMode_Testing;
}

void
rept_print_results(RepetitionTester *tester, FILE *fp)
{
	RepetitionTestResult *result = &tester->results;
	uint64_t cpu_freq = tester->cpu_timer_freq;
	print_row(fp, "Min", result->min, cpu_freq);
	fprintf(fp, "\n");
	print_row(fp, "Max", result->max, cpu_freq);
	fprintf(fp, "\n");
	print_row(fp, "Avg", result->total, cpu_freq);
	fprintf(fp, "\n");
}
