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

#include <random>
#include <climits>
extern "C" {
#include "bh_random.h"
}

static std::mt19937 generator;
static std::uniform_int_distribution<int> distribution(0, INT_MAX);

extern "C" void
bh_srand(unsigned int seed)
{
	generator.seed(seed);
}


extern "C" unsigned int
bh_rand(void)
{
	return distribution(generator);
}
