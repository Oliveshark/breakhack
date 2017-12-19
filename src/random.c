#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "random.h"

unsigned int
get_random(unsigned int max)
{
	static bool seeded = false;
	if (!seeded) {
		srand((unsigned int) time(NULL));
		seeded = true;
	}

	return rand() % (max + 1);
}
