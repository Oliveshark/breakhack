#ifndef REPETITION_TESTER_H_
#define REPETITION_TESTER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define rept_error(tester, msg) \
    do { \
        fprintf(stderr, "%s()[%d] %s\n", __func__, __LINE__, msg); \
        tester->mode = TestMode_Error; \
    } while (0)

typedef enum TestMode {
    TestMode_Uninitialized,
    TestMode_Testing,
    TestMode_Completed,
    TestMode_Error
} TestMode;

typedef enum RepetitionValueType {
    RepVal_test_count,
    RepVal_page_faults,
    RepVal_byte_count,
    RepVal_cpu_time,
    RepVal_count
} RepetitionValueType;

typedef struct RepetitionValue {
    uint64_t E[RepVal_count];
} RepetitionValue;

typedef struct RepetitionTestResult {
    RepetitionValue total;
    RepetitionValue min;
    RepetitionValue max;
} RepetitionTestResult;

typedef struct RepetitionTester {
    uint64_t target_processed_byte_count;
    uint64_t cpu_timer_freq;
    uint64_t test_time;
    uint64_t start_time;

    TestMode mode;
    bool print_new_mins;
    uint32_t open_block_count;
    uint32_t close_block_count;

    RepetitionValue current;
    RepetitionTestResult results;
} RepetitionTester;

void rept_setup(RepetitionTester *tester, uint64_t target_bytes,
        uint64_t cpu_timer_freq, uint32_t test_time);

void rept_begin(RepetitionTester *tester);

void rept_end(RepetitionTester *tester);

void rept_count_bytes(RepetitionTester *tester, uint64_t byte_count);

bool rept_is_testing(RepetitionTester *tester);

void rept_print_results(RepetitionTester *tester, FILE *fp);

#endif  // REPETITION_TESTER_H_

