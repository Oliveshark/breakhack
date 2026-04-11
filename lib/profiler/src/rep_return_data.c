#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <profiler.h>
#include <repetition_tester.h>

typedef struct TestFunction {
	const char *label;
	void (*func)(RepetitionTester *tester);
} TestFunction;

struct ReturnData {
	float fnum;
	size_t count;
	double dnum;
	float vec3[3];
};

typedef struct ReturnData ReturnData;

static ReturnData
inline_create_data(void)
{
	return (ReturnData){
	    .fnum = 1.2,
	    .count = 100,
	    .dnum = 3.4,
	    .vec3 = {1, 2, 3},
	};
}

static ReturnData
variable_create_data(void)
{
	ReturnData data;
	data.fnum = 1.2;
	data.count = 100;
	data.dnum = 3.4;
	data.vec3[0] = 1;
	data.vec3[1] = 2;
	data.vec3[2] = 3;
	return data;
}

static ReturnData *
malloc_create_data(void)
{
	ReturnData *data = malloc(sizeof(ReturnData));
	if (!data) {
		return NULL;
	}
	data->fnum = 1.2;
	data->count = 100;
	data->dnum = 3.4;
	data->vec3[0] = 1;
	data->vec3[1] = 2;
	data->vec3[2] = 3;
	return data;
}

static void
test_inline_static_data(RepetitionTester *tester)
{
	while (rept_is_testing(tester)) {
		rept_begin(tester);
		ReturnData data = inline_create_data();
		rept_end(tester);
		rept_count_bytes(tester, sizeof(data));
	}
}

static void
test_variable_static_data(RepetitionTester *tester)
{
	while (rept_is_testing(tester)) {
		rept_begin(tester);
		ReturnData data = variable_create_data();
		rept_end(tester);
		rept_count_bytes(tester, sizeof(data));
	}
}

static void
test_allocated_data(RepetitionTester *tester)
{
	while (rept_is_testing(tester)) {
		rept_begin(tester);
		ReturnData *data = malloc_create_data();
		rept_end(tester);
		free(data);
		rept_count_bytes(tester, sizeof(ReturnData));
	}
}

int
main(void)
{
	TestFunction tests[] = {
	    {"inline_static_data", test_inline_static_data},
	    {"variable_static_data", test_variable_static_data},
	    {"malloc_static_data", test_allocated_data},
	};
	const size_t test_count = sizeof(tests) / sizeof(TestFunction);
	RepetitionTester testers[sizeof(tests) / sizeof(TestFunction)] = {0};

	uint64_t cpu_freq = estimate_cpu_freq(2000);

	for (size_t i = 0; i < test_count; ++i) {
		RepetitionTester *tester = &testers[i];
		TestFunction *test_func = tests + i;

		printf("\n--- %s ---\n", test_func->label);

		rept_setup(tester, sizeof(ReturnData), cpu_freq, 10);
		test_func->func(tester);
	}

	return EXIT_SUCCESS;
}
