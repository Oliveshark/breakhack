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

#include "keyboardinput.h"
#include "keyboard.h"
#include "vector2d.h"

void
keyboardinput_init(KeyboardInput *input)
{
	input->currentState = 0;
	input->lastState = 0;
}

void
keyboardinput_handle_event(KeyboardInput *input, SDL_Event *event)
{
	if (keyboard_direction_press(UP, event)) {
		input->currentState |= KEY_UP;
	} else if (keyboard_direction_press(DOWN, event)) {
		input->currentState |= KEY_DOWN;
	} else if (keyboard_direction_press(LEFT, event)) {
		input->currentState |= KEY_LEFT;
	} else if (keyboard_direction_press(RIGHT, event)) {
		input->currentState |= KEY_RIGHT;
	} else if (keyboard_press(SDLK_0, event)) {
		input->currentState |= KEY_NUM1;
	}
	for (int i = SDLK_0; i <= SDLK_9; ++i) {
		if (keyboard_press(i, event))
			input->currentState |= (KEY_NUM0 << (i - SDLK_0));
	}

	if (keyboard_direction_release(UP, event)) {
		input->currentState &= ~KEY_UP;
	} else if (keyboard_direction_release(DOWN, event)) {
		input->currentState &= ~KEY_DOWN;
	} else if (keyboard_direction_release(LEFT, event)) {
		input->currentState &= ~KEY_LEFT;
	} else if (keyboard_direction_release(RIGHT, event)) {
		input->currentState &= ~KEY_RIGHT;
	} else if (keyboard_release(SDLK_0, event)) {
		input->currentState &= ~KEY_NUM1;
	}
	for (int i = SDLK_0; i <= SDLK_9; ++i) {
		if (keyboard_release(i, event))
			input->currentState &= ~(KEY_NUM0 << (i - SDLK_0));
	}
}

bool
key_is_pressed(KeyboardInput *input, Uint64 key)
{
	return (input->currentState & key) && !(input->lastState & key);
}

bool
key_is_released(KeyboardInput *input, Uint64 key)
{
	return (input->lastState & key) && !(input->currentState & key);
}

bool
key_is_down(KeyboardInput *input, Uint64 key)
{
	return input->currentState & key;
}

