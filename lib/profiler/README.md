# Profiler

A simple library to benchmark your code.

## Usage example

```c
#include <stdio.h>
#define PROFILER
#include "profiler.h"

static void random_func(void)
{
	TIME_FUNC_BEGIN();

	/* Your code goes here... */

	TIME_FUNC_END();
}

int main(int argc, char **argv)
{
	PROFILER_SETUP();
    (void) argc;
    (void) argv;

	random_func();

	TIME_BLOCK_BEGIN(block_name);
	/* Your code goes here */
	TIME_BLOCK_END(block_name);

	PROFILER_STOP(stdout);
}
```

### Example output

```text
                      =====   Benchmarks :   =====
                              Total time : 106691427524 (34.7301 s)
                   run_game_update[1933] : 20781332     0.02% (2.53% w/children)
     update_before_particle_engine[1933] : 195193852    0.18%
      update_after_particle_engine[1933] : 1123800376   1.05%
            particle_engine_update[1933] : 1354889225   1.27%
```

## How it works

Under the hood `benchmark` is using the `RDTSC` assembly instruction to time
things. Then there's a bunch of other logic that I don't remember because it's
been a while since I wrote it.
