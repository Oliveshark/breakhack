#ifndef __MACROS_H_
#define __MACROS_H_

#include "internal/profiler_c.h"
#include "rdtsc.h"

#ifdef PROFILER

#define PROFILER_SETUP() \
    prof_start();

#define PROFILER_STOP(fp) \
    prof_stop(); \
    prof_print(fp);

#define _NameConcat(A, B) A##B
#define NameConcat(A, B) _NameConcat(A, B)

#define TIME_BANDWIDTH_BEGIN(label, bytes) \
    AnchorBlock NameConcat(Block, label) = make_anchor_block(#label, bytes);

#define TIME_BANDWIDTH_END(label) \
    read_anchor_block( &Block##label );

#define TIME_BLOCK_BEGIN(label) TIME_BANDWIDTH_BEGIN(label, 0)
#define TIME_BLOCK_END(label) TIME_BANDWIDTH_END(label)

#define TIME_FUNC_BEGIN() \
    AnchorBlock NameConcat(Block, __func__) = make_anchor_block(__func__, 0);

#define TIME_FUNC_END() \
    read_anchor_block( &Block##__func__ );

#else
#define PROFILER_SETUP()
#define PROFILER_STOP(fp)
#define TIME_BLOCK_BEGIN(label)
#define TIME_BLOCK_END(label)
#define TIME_BANDWIDTH_BEGIN(label, bytes)
#define TIME_BANDWIDTH_END(label)
#define TIME_FUNC_BEGIN()
#define TIME_FUNC_END()
#endif


#endif // __MACROS_H_

