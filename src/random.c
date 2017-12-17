#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

unsigned int
get_random(unsigned int max)
{
	srand(time(NULL));
	return rand() % (max + 1);
}
