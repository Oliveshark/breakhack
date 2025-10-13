#define PROFILER

#include "common.h"
#include "profiler_c.h"
#include "rdtsc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

static Profiler _Prof;
static char *CurrentAnchorLabel = NULL;

inline static uint64_t hash_key(const char *key)
{
    uint64_t hash = FNV_OFFSET;
    for (const char *p = key; *p; ++p) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

inline static size_t hash_index(const char *key)
{
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (ANCHOR_CAPACITY - 1));
    return index;
}

inline static ProfileAnchor* get_anchor(const char *label)
{
    size_t index = hash_index(label);
    ProfileAnchor *cp = _Prof.anchors + index;

    while (cp->label != NULL && strcmp(cp->label, label) != 0) {
        cp++;
    }

    if (cp->label == NULL) {
        cp->label = label;
        cp->elapsed_inclusive = 0;
        cp->elapsed_exclusive = 0;
        _Prof.len++;
    }
    return cp;
}

void prof_start(void)
{
    _Prof.start = read_cpu_timer();
}

void prof_stop(void)
{
    _Prof.stop = read_cpu_timer();
}

static void prof_add_child_anchor(const char *label, uint64_t elapsed)
{
    ProfileAnchor *cp = get_anchor(label);

    cp->elapsed_exclusive -= elapsed;
}

void prof_add_anchor(const AnchorBlock *anchor, uint64_t elapsed)
{
    ProfileAnchor *cp = get_anchor(anchor->label);

    if (anchor->parent_anchor != NULL) {
        prof_add_child_anchor(anchor->parent_anchor, elapsed);
    }

    cp->elapsed_exclusive += elapsed;
    cp->elapsed_inclusive = anchor->old_elapsed_inclusive + elapsed;
    cp->processed_byte_count += anchor->processed_byte_count;
    cp->hits++;

    if (_Prof.len > ANCHOR_CAPACITY / 2) {
        fprintf(stderr, "%s: Profile anchors beyond optimal count: %zu > %d\n", __func__, _Prof.len, ANCHOR_CAPACITY);
    }
}

static void print_header(FILE *fp, int indent, const char *time_label, uint64_t elapsed, uint64_t cpu_freq)
{
    if (isatty(fileno(fp))) {
        fprintf(fp, C_BLUE "\n%*s :   =====\n" C_RESET, indent, "=====   Benchmarks");
        fprintf(fp, C_RED "%*s" C_RESET " : %lu " C_WHITE "(%.4fs with freq %lu)" C_RESET "\n",
                indent, time_label, elapsed, (float)elapsed/cpu_freq, cpu_freq);
    } else {
        fprintf(fp, "\n%*s :   =====\n", indent, "=====   Benchmarks");
        fprintf(fp, "%*s : %lu (%.4f s)\n", indent, time_label, elapsed, (float)elapsed/cpu_freq);
    }
}

static void print_cp(int indent, const char *label, uint64_t elapsed, double perc, FILE *fp)
{
    if (isatty(fileno(fp))) {
        fprintf(fp, C_GREEN "%*s" C_RESET " : %-11lu " C_YELLOW "%6.02f%%" C_RESET,
                indent, label, elapsed, perc);
    } else {
        fprintf(fp, "%*s : %-10lu %6.02f%%", indent, label, elapsed, perc);
    }
}

static void print_cp_child(int indent, const char *label, uint64_t elapsed, double perc, double total_perc, FILE *fp)
{
    if (isatty(fileno(fp))) {
        fprintf(fp, C_GREEN "%*s" C_RESET " : %-11lu " C_YELLOW "%6.02f%% (%.02f%% w/children)" C_RESET,
                indent, label, elapsed, perc, total_perc);
    } else {
        fprintf(fp, "%*s : %-10lu %6.02f%% (%.02f%% w/children)", indent, label, elapsed, perc, total_perc);
    }
}

void prof_print(FILE *fp)
{
    char label[256];
    uint64_t total_elapsed = 0;
    uint64_t total = _Prof.stop - _Prof.start;
    double share, total_share;
    ProfileAnchor *cp;
    uint64_t cpu_freq = estimate_cpu_freq(300);
    char *time_label = "Total time";

    int indent = strlen(time_label);
    for (size_t i = 0; i < ANCHOR_CAPACITY; ++i) {
        cp = _Prof.anchors + i;
        if (cp->hits == 0) {
            continue;
        }
        sprintf(label, "%s[%zu]", cp->label, cp->hits);
        int len = strlen(label);
        indent = len > indent ? len : indent;
    }
    indent += 5;

    total_elapsed = _Prof.stop - _Prof.start;
    print_header(fp, indent, time_label, total_elapsed, cpu_freq);

    for (size_t i = 0; i < ANCHOR_CAPACITY; ++i) {
        cp = _Prof.anchors + i;
        if (cp->hits == 0) {
            continue;
        }
        sprintf(label, "%s[%zu]", cp->label, cp->hits);
        share = ((double) cp->elapsed_exclusive/(double) total) * 100;
        if (cp->elapsed_inclusive == cp->elapsed_exclusive) {
            print_cp(indent, label, cp->elapsed_exclusive, share, fp);
        } else {
            total_share = ((double) cp->elapsed_inclusive/(double) total) * 100;
            print_cp_child(indent, label, cp->elapsed_exclusive, share, total_share, fp);
        }
        if (cp->processed_byte_count) {
            static const double mb = 1024.0*1024.0;
            static const double gb = mb*1024.0;

            double seconds = (double) cp->elapsed_inclusive / cpu_freq;
            double bytes_per_second = cp->processed_byte_count / seconds;
            double mbs = cp->processed_byte_count / mb;
            double gb_per_second = bytes_per_second / gb;

            if (isatty(fileno(fp))) {
                printf(C_CYAN " %.3fmb" C_RESET " at " C_MAGENTA "%.2fgb/s" C_RESET, mbs, gb_per_second);
            } else {
                printf(" %.3fmb at %.2fgb/s", mbs, gb_per_second);
            }
        }
        fprintf(fp, "\n");
    }
}

AnchorBlock make_anchor_block(const char *label, size_t used_bytes)
{
    ProfileAnchor *cp = get_anchor(label);
    AnchorBlock anchor = { label, read_cpu_timer(), cp->elapsed_inclusive, used_bytes, CurrentAnchorLabel };
    CurrentAnchorLabel = (char*) label;
    return anchor;
}

void read_anchor_block(const AnchorBlock *anchor)
{
    uint64_t elapsed = read_cpu_timer() - anchor->start;
    prof_add_anchor(anchor, elapsed);
    CurrentAnchorLabel = anchor->parent_anchor;
}

