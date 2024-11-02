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
	input->lastMouseX = 0;
	input->lastMouseY = 0;
	input->modKeyState = 0;
	input->lastModKeyState = 0;
}

void input_reset(Input *input)
{
	input->lastKeyState = input->keyState;
	input->lastMouseButtonState = input->mouseButtonState;
	input->lastModKeyState = input->modKeyState;
	input->keyState = 0;
	input->mouseButtonState = 0;
	input->modKeyState = 0;
	input->lastMouseX = input->mouseX;
	input->lastMouseY = input->mouseY;
}

static Uint64
get_event_key(SDL_Event *event)
{
	Uint64 key;
	switch (event->key.key) {
		case SDLK_UP:
		case SDLK_K :
		case SDLK_W :
			key = KEY_UP; break;
		case SDLK_DOWN:
		case SDLK_J :
		case SDLK_S :
			key = KEY_DOWN; break;
		case SDLK_LEFT:
		case SDLK_H :
		case SDLK_A :
			key = KEY_LEFT; break;
		case SDLK_RIGHT:
		case SDLK_L :
		case SDLK_D :
			key = KEY_RIGHT; break;
		case SDLK_0:
			key = KEY_NUM0; break;
		case SDLK_1:
			key = KEY_NUM1; break;
		case SDLK_2:
			key = KEY_NUM2; break;
		case SDLK_3:
			key = KEY_NUM3; break;
		case SDLK_4:
			key = KEY_NUM4; break;
		case SDLK_5:
			key = KEY_NUM5; break;
		case SDLK_ESCAPE:
			key = KEY_ESC; break;
		case SDLK_RETURN:
			key = KEY_ENTER; break;
		case SDLK_SPACE:
			key = KEY_SPACE; break;
		default:
			key = 0; break;
	}
	return key;
}

static Uint64
get_event_button(SDL_Event *event)
{
	Uint64 key;
	switch (event->jbutton.button) {
		case SDL_GAMEPAD_BUTTON_DPAD_UP :
			key = KEY_UP; break;
		case SDL_GAMEPAD_BUTTON_DPAD_DOWN :
			key = KEY_DOWN; break;
		case SDL_GAMEPAD_BUTTON_DPAD_LEFT :
			key = KEY_LEFT; break;
		case SDL_GAMEPAD_BUTTON_DPAD_RIGHT :
			key = KEY_RIGHT; break;
		case SDL_GAMEPAD_BUTTON_SOUTH :
			key = KEY_NUM1 | KEY_ENTER; break;
		case SDL_GAMEPAD_BUTTON_WEST :
			key = KEY_NUM2; break;
		case SDL_GAMEPAD_BUTTON_NORTH :
			key = KEY_NUM3; break;
		case SDL_GAMEPAD_BUTTON_EAST :
			key = KEY_NUM4; break;
		case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER :
			key = KEY_NUM5; break;
		case SDL_GAMEPAD_BUTTON_START :
			key = KEY_ENTER; break;
		case SDL_GAMEPAD_BUTTON_BACK :
			key = KEY_ESC; break;
		case SDL_GAMEPAD_BUTTON_LEFT_STICK :
		case SDL_GAMEPAD_BUTTON_RIGHT_STICK :
			key = KEY_SPACE; break;
		default:
			key = 0; break;
	}
	return key;
}

static Uint64
get_axis_motion(SDL_Event *event)
{
	Uint64 key;
	int angle = event->gaxis.value;
	switch (event->gaxis.axis) {
		case SDL_GAMEPAD_AXIS_LEFTX :
			key = angle < 0 ? KEY_LEFT : KEY_RIGHT; break;
		case SDL_GAMEPAD_AXIS_LEFTY :
			key = angle < 0 ? KEY_UP : KEY_DOWN; break;
		default:
			key = 0;
	}
	return key;
}

static Uint32
get_event_modkey(SDL_Event *event)
{
	Uint32 key = 0;
	if (event->key.mod & (SDL_KMOD_LCTRL | SDL_KMOD_RCTRL)) {
		switch (event->key.key) {
			case SDLK_S :
				key = KEY_CTRL_S; break;
			case SDLK_M :
				key = KEY_CTRL_M; break;
			case SDLK_D :
				key = KEY_CTRL_D; break;
			case SDLK_F :
				key = KEY_CTRL_F; break;
		}
	} else if (event->key.mod & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) {
		switch (event->key.key) {
			case SDLK_1:
				key = KEY_SHIFT_NUM1; break;
			case SDLK_2:
				key = KEY_SHIFT_NUM2; break;
			case SDLK_3:
				key = KEY_SHIFT_NUM3; break;
			case SDLK_4:
				key = KEY_SHIFT_NUM4; break;
			case SDLK_5:
				key = KEY_SHIFT_NUM5; break;
		}
	}
	return key;
}

static Uint32
get_event_mousebutton(SDL_Event *event)
{
	Uint32 key = 0;
	switch (event->button.button) {
		case SDL_BUTTON_LEFT:
			key = MBUTTON_LEFT; break;
		case SDL_BUTTON_MIDDLE:
			key = MBUTTON_MIDDLE; break;
		case SDL_BUTTON_RIGHT:
			key = MBUTTON_RIGHT; break;
		default:
			key = 0;
	}
	return key;
}

void
input_handle_event(Input *input, SDL_Event *event, InputDeviceType *device_type)
{
	InputDeviceType current_device_type = DeviceType_Unknown;

	if (event->type == SDL_EVENT_KEY_DOWN) {
		Uint32 key;
		if ((key = get_event_modkey(event))) {
			input->modKeyState |= key;
		} else {
			input->keyState |= get_event_key(event);
		}
		current_device_type = DeviceType_Keyboard;
	} else if (event->type == SDL_EVENT_KEY_UP) {
		Uint32 key;
		if ((key = get_event_modkey(event))) {
			input->modKeyState &= ~key;
		} else {
			input->keyState &= ~get_event_key(event);
		}
		current_device_type = DeviceType_Keyboard;
	} else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		input->mouseButtonState |= get_event_mousebutton(event);
	} else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		input->mouseButtonState &= ~get_event_mousebutton(event);
	} else if (event->type == SDL_EVENT_MOUSE_MOTION) {
		input->mouseX = (Uint32) event->motion.x;
		input->mouseY = (Uint32) event->motion.y;
	} else if (event->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
		input->keyState |= get_event_button(event);
		current_device_type = DeviceType_Gamepad;
	} else if (event->type == SDL_EVENT_GAMEPAD_BUTTON_UP) {
		input->keyState &= ~get_event_button(event);
		current_device_type = DeviceType_Gamepad;
	} else if (event->type == SDL_EVENT_GAMEPAD_AXIS_MOTION) {
		if (event->gaxis.value > 31500 || event->gaxis.value < -31500) {
			input->keyState |= get_axis_motion(event);
		} else {
			input->keyState &= ~get_axis_motion(event);
		}
	}

	if (device_type != NULL) {
		*device_type = current_device_type;
	}
}

bool
input_key_is_pressed(Input *input, Uint64 key)
{
	return (input->keyState & key) && !(input->lastKeyState & key);
}

bool
input_modkey_is_pressed(Input *input, Uint32 key)
{
	return (input->modKeyState & key)
		&& !(input->lastModKeyState & key);
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

bool
input_mouse_moved(Input *input)
{
	return (input->mouseX != input->lastMouseX)
		|| (input->mouseY != input->lastMouseY);
}
