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

#include "gamecontroller.h"
#include "util.h"

static SDL_Gamepad *controller = NULL;
static SDL_Haptic *haptic = NULL;
static Uint8 controllerMode = GAMEPAD_TYPE_NONE;

void
gamecontroller_set(SDL_Gamepad *ctrler)
{
	if (ctrler == NULL) {
		info("Game controller disconnected");
		if (controller) {
			SDL_CloseGamepad(controller);
			controller = NULL;
		}
		if (haptic) {
			SDL_CloseHaptic(haptic);
			haptic = NULL;
		}
		controllerMode = GAMEPAD_TYPE_NONE;
		return;
	}

	controller = ctrler;

	const char *ctrlName = SDL_GetGamepadName(controller);
	info("Game controller connected: %s", ctrlName);

	// Try to determine if this is a PS3/4 controller
	if (ctrlName[0] == 'P' &&
	    ctrlName[1] == 'S' &&
	    (ctrlName[2] == '4' || ctrlName[2] == '3'))
		controllerMode = GAMEPAD_TYPE_PS;
	else
		controllerMode = GAMEPAD_TYPE_XB;

	haptic = SDL_OpenHapticFromJoystick(SDL_GetGamepadJoystick(controller));
	if (haptic) {
		info("Haptics are supported by controller: %s", ctrlName);
		if (SDL_InitHapticRumble(haptic)) {
			info("Haptics enabled for controller: %s", ctrlName);
		} else {
			info("Failed to enable haptics for: %s", ctrlName);
		}
	} else {
		info("Haptics not supported by controller: %s", ctrlName);
	}
}

void
gamecontroller_rumble(float intensity, Uint32 duration)
{
	if (controllerMode == GAMEPAD_TYPE_NONE)
		return;

	if (!haptic)
		return;

	if (SDL_PlayHapticRumble(haptic, intensity, duration) != 0)
		error("Failed to play rumble: %s", SDL_GetError());
}

Uint8
gamecontroller_mode(void)
{
	return controllerMode;
}

void
gamecontroller_close(void)
{
	if (controller)
		SDL_CloseGamepad(controller);
	if (haptic)
		SDL_CloseHaptic(haptic);
}
