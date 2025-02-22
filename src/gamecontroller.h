/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2025  Linus Probert <linus.probert@gmail.com>
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

#pragma once

#include <SDL3/SDL.h>

typedef enum {
	GAMEPAD_TYPE_NONE,
	GAMEPAD_TYPE_XB,
	GAMEPAD_TYPE_PS,
	GAMEPAD_TYPE_MAX
} GamepadType;

typedef struct GameController {
	SDL_Gamepad *controller;
	unsigned int mode;
} GameController;

void
gamecontroller_set(SDL_Gamepad *controller);

void
gamecontroller_rumble(float intensity, Uint32 duration);

Uint8
gamecontroller_mode(void);

void
gamecontroller_close(void);
