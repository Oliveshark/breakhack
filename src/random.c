#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

unsigned int
get_random(unsigned int max)
{
	static bool seeded = false;
	if (!seeded) {
		srand(time(NULL));
		seeded = true;
	}

	return rand() % (max + 1);
}
