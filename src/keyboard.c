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

#include "keyboard.h"
#include "util.h"

static bool
extract_key(Direction dir, SDL_Event *event)
{
	Uint32 key = event->key.key;
	switch (dir) {
		case UP:
			return key == SDLK_UP
				|| key == SDLK_W
				|| key == SDLK_K;
		case DOWN:
			return key == SDLK_DOWN
				|| key == SDLK_S
				|| key == SDLK_J;
		case LEFT:
			return key == SDLK_LEFT
				|| key == SDLK_A
				|| key == SDLK_H;
		case RIGHT:
			return key == SDLK_RIGHT
				|| key == SDLK_L
				|| key == SDLK_D;
		default:
			return false;
	}
}

bool
keyboard_direction_press(Direction dir, SDL_Event *event)
{
	if (event->type != SDL_EVENT_KEY_DOWN)
		return false;

	return extract_key(dir, event);
}

bool
keyboard_direction_release(Direction dir, SDL_Event *event)
{
	if (event->type != SDL_EVENT_KEY_UP)
		return false;

	return extract_key(dir, event);
}

bool
keyboard_press(Uint32 key, SDL_Event *event)
{
	if (event->type != SDL_EVENT_KEY_DOWN)
		return false;

	return key == (Uint32) event->key.key;
}

bool
keyboard_release(Uint32 key, SDL_Event *event)
{
	if (event->type != SDL_EVENT_KEY_UP)
		return false;

	return key == (Uint32) event->key.key;
}

bool
keyboard_mod_press(Uint32 key, Uint32 mod, SDL_Event *event)
{
	if (event->type != SDL_EVENT_KEY_DOWN)
		return false;

	if (!(event->key.mod & mod))
		return false;

	return key == (Uint32) event->key.key;
}

