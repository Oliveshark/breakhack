/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "random.h"
#include "util.h"

static unsigned int seed = 0;
static unsigned int map_seeds[20];
static unsigned int runtime_seed = 0;

static void
generate_random_seeds(void)
{
	// Use seed for generating map seeds
	srand(seed);
	info("Core random seed: %d", seed);
	for (int i = 0; i < 20; ++i) {
		map_seeds[i] = rand();
	}

	// Set a more random seed for runtime random
	runtime_seed = (unsigned int) time(NULL);
	srand(runtime_seed);
	info("Runtime random seed: %d", runtime_seed);
}

static void
init_seed(void)
{
	if (seed == 0) {
		seed = (unsigned int) time(NULL);
		generate_random_seeds();
	}
}

void
set_random_seed(unsigned int s)
{
	seed = s;
	generate_random_seeds();
}

unsigned int
get_random_seed(void)
{
	init_seed();
	return seed;
}

unsigned int
get_random_map_seed(unsigned int level)
{
	return map_seeds[level-1];
}

unsigned int
get_random(unsigned int max)
{
	init_seed();
	return rand() % (max + 1);
}
