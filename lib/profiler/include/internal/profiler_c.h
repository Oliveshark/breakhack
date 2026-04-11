#ifndef _STOP_CLOCK_H_
#define _STOP_CLOCK_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define ANCHOR_CAPACITY 4096

typedef struct ProfileAnchor {
	const char *label;
	size_t hits;
	uint64_t elapsed_exclusive;
	uint64_t elapsed_inclusive;
	size_t processed_byte_count;
} ProfileAnchor;

typedef struct Profiler {
	uint64_t start, stop;
	size_t len;
	ProfileAnchor anchors[ANCHOR_CAPACITY];
} Profiler;

typedef struct AnchorBlock {
	const char *label;
	uint64_t start;
	uint64_t old_elapsed_inclusive;
	size_t processed_byte_count;
	char *parent_anchor;
} AnchorBlock;

void prof_init(void);

void prof_start(void);

void prof_stop(void);

void prof_add_anchor(const AnchorBlock *anchor, uint64_t elapsed);

void prof_print(FILE *fp);

AnchorBlock make_anchor_block(const char *label, size_t used_bytes);
void read_anchor_block(const AnchorBlock *anchor);

#endif // _STOP_CLOCK_H_
