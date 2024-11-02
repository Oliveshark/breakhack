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

#ifndef TIMER_H_
#define	TIMER_H_

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct Timer {
	Uint64 startTime;
} Timer;

Timer* _timer_create(void);
void timer_start(Timer*);
void timer_stop(Timer*);
bool timer_started(Timer*);
Uint64 timer_get_ticks(Timer*);
void timer_destroy(Timer*);

#endif // TIMER_H_
