# Benchmark

A simple library to benchmark your code.

## Compiling

- `mkdir build`
- `cd build`
- `cmake ..`
- `make`

## Usage example

```c
#include <stdio.h>
#include "benchmark.h"

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    /* Create an initate the stopclock */
    StopClock sclock;
    sclock_init(&sclock);

    sclock_start(&sclock);

    /* Execute some code */

    sclock_checkpoint(&sclock, "First part");

    /* Execute some more code */

    sclock_checkpoint(&sclock, "Second part");

    /* Execute even more code */

    sclock_checkpoint(&sclock, "Third part");

    /* Stop the clock */
    sclock_stop(&sclock);

    /* Output the results */
    sclock_print(&sclock, stdout);

    /* Clean up */
    sclock_destroy(&sclock);
}
```

### Example output

```
         =====   Benchmarks :   =====
                 First part : 5916119358   95.54%
                Second part : 6940092       0.17%
                 Third part : 265775256     4.29%
             Remaining time : 3478810       0.00%
```

## How it works

Under the hood `benchmark` is using the `RDTSC` assembly instruction to time
things. During the `sclock_init` the number of pseudo clocks used in `RDTSC`
occur in one second (over 300ms). This is a rough estimate but usually aligns
well with the Ghz number specified on the running computers cpu.

## API

> TODO: Generate API and link to it here
