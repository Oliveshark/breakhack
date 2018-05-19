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

#ifndef INPUT_H_
#define	INPUT_H_

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
#define KEY_ESC		16384
#define KEY_ENTER	32768
#define KEY_CTRL	65536
#define KEY_ALT		131072
#define KEY_M		262144
#define KEY_S		524288

#define MBUTTON_LEFT	1
#define MBUTTON_MIDDLE	2
#define MBUTTON_RIGHT	4

typedef struct Input {
	Uint64 keyState;
	Uint64 lastKeyState;
	Uint32 mouseButtonState;
	Uint32 lastMouseButtonState;
	Uint32 mouseX;
	Uint32 mouseY;
} Input;

void
input_init(Input *);

void
input_reset(Input *);

void
input_handle_event(Input *, SDL_Event*);

bool
input_key_is_pressed(Input *, Uint64 key);

bool
input_key_is_released(Input *, Uint64 key);

bool
input_key_is_down(Input *, Uint64 key);

bool
input_mousebutton_is_pressed(Input *, Uint8 button);

#endif // INPUT_H_

