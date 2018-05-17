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

#ifndef KEYBOARDINPUT_H_
#define	KEYBOARDINPUT_H_

#include <SDL.h>
#include <stdbool.h>

#define KEY_LEFT	1
#define KEY_RIGHT	2
#define KEY_UP		4
#define KEY_DOWN	8
#define KEY_NUM0	16
#define KEY_NUM1	32
#define KEY_NUM2	64
#define KEY_NUM3	128
#define KEY_NUM4	256
#define KEY_NUM5	512
#define KEY_NUM6	1024
#define KEY_NUM7	2048
#define KEY_NUM8	4096
#define KEY_NUM9	8192

typedef struct KeyboardState {
	bool dir_left;
	bool dir_right;
	bool dir_up;
	bool dir_down;
} KeyboardState;

typedef struct KeyboardInput {
	Uint64 currentState;
	Uint64 lastState;
} KeyboardInput;

void
keyboardinput_init(KeyboardInput *);

void
keyboardinput_handle_event(KeyboardInput *, SDL_Event*);

bool
key_is_pressed(KeyboardInput *, Uint64 key);

bool
key_is_released(KeyboardInput *, Uint64 key);

bool
key_is_down(KeyboardInput *, Uint64 key);

#endif // KEYBOARDINPUT_H_

