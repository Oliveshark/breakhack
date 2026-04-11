#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <profiler.h>
#include <repetition_tester.h>

typedef enum AllocationType {
	AllocType_none,
	AllocType_malloc,
	AllocType_count,
} AllocationType;

typedef struct ReadParams {
	const char *fname;
	AllocationType alloc_type;
	uint8_t *buffer;
	size_t buflen;
} ReadParams;

typedef struct TestFunction {
	const char *label;
	void (*func)(RepetitionTester *tester, ReadParams *params);
} TestFunction;

static const char *
print_allocation_type(AllocationType type)
{
	const char *result;
	switch (type) {
		case AllocType_none:
			result = "";
			break;
		case AllocType_malloc:
			result = "malloc";
			break;
		default:
			result = "unknown";
			break;
	}
	return result;
}

static void
handle_allocation(ReadParams *params, uint8_t **buffer)
{
	switch (params->alloc_type) {
		case AllocType_malloc:
			*buffer = malloc(params->buflen);
			break;
		case AllocType_none:
			break;
		default:
			fprintf(stderr, "ERROR: Unrecognized allocation type");
			break;
	}
}

static void
handle_deallocation(ReadParams *params, uint8_t *buffer)
{
	switch (params->alloc_type) {
		case AllocType_malloc:
			free(buffer);
			break;
		case AllocType_none:
			break;
		default:
			fprintf(stderr, "ERROR: Unrecognized allocation type");
			break;
	}
}

static void
test_fread(RepetitionTester *tester, ReadParams *params)
{
	while (rept_is_testing(tester)) {
		FILE *fp = fopen(params->fname, "r");
		if (fp == NULL) {
			rept_error(tester, "fopen failed");
		} else {
			uint8_t *dest = params->buffer;
			handle_allocation(params, &dest);
			rept_begin(tester);
			size_t result = fread(dest, sizeof(uint8_t), params->buflen, fp);
			rept_end(tester);

			if (result == params->buflen) {
				rept_count_bytes(tester, result);
			} else {
				rept_error(tester, "fread failed");
			}
			handle_deallocation(params, dest);
			fclose(fp);
		}
	}
}

static void
test_read(RepetitionTester *tester, ReadParams *params)
{
	while (rept_is_testing(tester)) {
		int fd = open(params->fname, O_RDONLY);
		if (fd < 0) {
			rept_error(tester, "open failed");
		} else {
			ssize_t read_bytes = 0;
			uint8_t *dest = params->buffer;
			handle_allocation(params, &dest);
			rept_begin(tester);
			while (read_bytes < (ssize_t)params->buflen) {
				ssize_t bytes = read(fd, dest, params->buflen - read_bytes);
				rept_count_bytes(tester, bytes);
				if (bytes < 0) {
					rept_error(tester, "read failed");
				}
				read_bytes += bytes;
			}
			rept_end(tester);
			handle_deallocation(params, dest);
			close(fd);
		}
	}
}

int
main(int argc, char **argv)
{

	TestFunction tests[] = {{"fread", test_fread}, {"read", test_read}};
	const char *fname;
	size_t fsize;

	if (argc != 2) {
		fprintf(stderr, "USAGE: %s <file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	fname = argv[1];

	struct stat statbuf;
	if (stat(fname, &statbuf) != 0) {
		perror("stat");
		return EXIT_FAILURE;
	}
	fsize = statbuf.st_size;

	const size_t test_count = sizeof(tests) / sizeof(TestFunction);
	RepetitionTester testers[sizeof(tests) / sizeof(TestFunction)][AllocType_count] = {0};
	ReadParams params = {
	    .buffer = malloc(fsize * sizeof(uint8_t)),
	    .buflen = fsize,
	    .fname = fname,
	};

	uint64_t cpu_freq = estimate_cpu_freq(1000);

	if (params.buflen > 0) {
		// while (1) {
		for (size_t i = 0; i < test_count; ++i) {
			for (uint32_t alloc_type = AllocType_none; alloc_type < AllocType_count; ++alloc_type) {
				RepetitionTester *tester = &testers[i][alloc_type];
				TestFunction *test_func = tests + i;
				params.alloc_type = alloc_type;

				printf("\n--- %s%s%s ---\n", print_allocation_type(alloc_type),
				       params.alloc_type ? " + " : "", test_func->label);

				rept_setup(tester, fsize, cpu_freq, 10);
				test_func->func(tester, &params);
			}
		}
		//}
	}

	free(params.buffer);

	return EXIT_SUCCESS;
}
