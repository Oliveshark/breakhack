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

#include "input.h"
#include "vector2d.h"

void
input_init(Input *input)
{
	input->keyState = 0;
	input->lastKeyState = 0;
	input->mouseButtonState = 0;
	input->lastMouseButtonState = 0;
	input->mouseX = 0;
	input->mouseY = 0;
}

void input_reset(Input *input)
{
	input->lastKeyState = input->keyState;
	input->lastMouseButtonState = input->mouseButtonState;
	input->keyState = 0;
	input->mouseButtonState = 0;
}

static Uint64
get_event_key(SDL_Event *event)
{
	switch (event->key.keysym.sym) {
		case SDLK_UP:
		case SDLK_k:
		case SDLK_w:
			return KEY_UP;
		case SDLK_DOWN:
		case SDLK_j:
			return KEY_DOWN;
		case SDLK_s:
			return KEY_DOWN | KEY_S;
		case SDLK_LEFT:
		case SDLK_h:
		case SDLK_a:
			return KEY_LEFT;
		case SDLK_RIGHT:
		case SDLK_l:
		case SDLK_d:
			return KEY_RIGHT;
		case SDLK_0:
			return KEY_NUM0;
		case SDLK_1:
			return KEY_NUM1;
		case SDLK_2:
			return KEY_NUM2;
		case SDLK_3:
			return KEY_NUM3;
		case SDLK_4:
			return KEY_NUM4;
		case SDLK_5:
			return KEY_NUM5;
		case SDLK_6:
			return KEY_NUM6;
		case SDLK_7:
			return KEY_NUM7;
		case SDLK_8:
			return KEY_NUM8;
		case SDLK_9:
			return KEY_NUM9;
		case SDLK_ESCAPE:
			return KEY_ESC;
		case SDLK_RETURN:
			return KEY_ENTER;
		case SDLK_LALT:
		case SDLK_RALT:
			return KEY_ALT;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			return KEY_CTRL;
		case SDLK_m:
			return KEY_M;
		default:
			return 0;
	}
}

static Uint32
get_event_mousebutton(SDL_Event *event)
{
	switch (event->button.button) {
		case SDL_BUTTON_LEFT:
			return MBUTTON_LEFT;
		case SDL_BUTTON_MIDDLE:
			return MBUTTON_MIDDLE;
		case SDL_BUTTON_RIGHT:
			return MBUTTON_RIGHT;
		default:
			return 0;
	}
}

void
input_handle_event(Input *input, SDL_Event *event)
{
	if (event->type == SDL_KEYDOWN)
		input->keyState |= get_event_key(event);
	else if (event->type == SDL_KEYUP) 
		input->keyState &= ~get_event_key(event);
	else if (event->type == SDL_MOUSEBUTTONDOWN)
		input->mouseButtonState |= get_event_mousebutton(event);
	else if (event->type == SDL_MOUSEBUTTONUP)
		input->mouseButtonState &= ~get_event_mousebutton(event);
	else if (event->type == SDL_MOUSEMOTION) {
		input->mouseX = event->motion.x;
		input->mouseY = event->motion.y;
	}
}

bool
input_key_is_pressed(Input *input, Uint64 key)
{
	return (input->keyState & key) && !(input->lastKeyState & key);
}

bool
input_key_is_released(Input *input, Uint64 key)
{
	return (input->lastKeyState & key) && !(input->keyState & key);
}

bool
input_key_is_down(Input *input, Uint64 key)
{
	return input->keyState & key;
}

bool
input_mousebutton_is_pressed(Input *input, Uint8 button)
{
	return (input->mouseButtonState & button)
		&& !(input->lastMouseButtonState & button);
}
