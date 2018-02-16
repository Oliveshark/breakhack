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

#include <SDL2/SDL.h>
#include <stdlib.h>
#include "util.h"
#include "timer.h"

Timer* timer_create()
{
	Timer *t = ec_malloc(sizeof(Timer));
	t->startTime = 0;
	return t;
}

void timer_start(Timer *t)
{
	t->startTime = SDL_GetTicks();
}

void timer_stop(Timer *t)
{
	t->startTime = 0;
}

bool timer_started(Timer *t)
{
	return t->startTime != 0;
}

unsigned int timer_get_ticks(Timer *t)
{
	if (!t->startTime)
		return 0;

	return SDL_GetTicks() - t->startTime;
}

void timer_destroy(Timer *t)
{
	free(t);
}
