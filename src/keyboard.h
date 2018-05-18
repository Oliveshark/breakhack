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

#ifndef KEYBOARD_H_
#define	KEYBOARD_H_

#include <SDL_events.h>
#include <stdbool.h>
#include "defines.h"

bool
keyboard_direction_press(Direction, SDL_Event*);

bool
keyboard_direction_release(Direction, SDL_Event*);

bool
keyboard_press(Uint32 key, SDL_Event*);

bool
keyboard_release(Uint32 key, SDL_Event*);

bool
keyboard_mod_press(Uint32 key, Uint32 mod, SDL_Event*);

#endif // KEYBOARD_H_
