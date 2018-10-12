#include "gamecontroller.h"
#include "util.h"

static SDL_GameController *controller = NULL;
static SDL_Haptic *haptic = NULL;
static Uint8 controllerMode = 0;

void
gamecontroller_set(SDL_GameController *ctrler)
{
	controller = ctrler;

	const char *ctrlName = SDL_GameControllerName(controller);
	info("Game controller connected: %s", ctrlName);

	// Try to determine if this is a PS3/4 controller
	if (ctrlName[0] == 'P' &&
	    ctrlName[1] == 'S' &&
	    (ctrlName[2] == '4' || ctrlName[2] == '3'))
		controllerMode = 2;
	else
		controllerMode = 1;

	haptic = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(controller));
	if (haptic) {
		info("Haptics are supported by controller: %s", ctrlName);
		if (SDL_HapticRumbleInit(haptic) >= 0) {
			info("Haptics enabled for controller: %s", ctrlName);
		}
		else {
			info("Failed to enable haptics for: %s", ctrlName);
		}
	}
	else {
		info("Haptics not supported by controller: %s", ctrlName);
	}
}

void
gamecontroller_rumble(float intensity, Uint32 duration)
{
	if (!haptic)
		return;

	if (SDL_HapticRumblePlay(haptic, intensity, duration) != 0)
		error("Failed to play rumble: %s", SDL_GetError());
}

Uint8
gamecontroller_mode()
{
	return controllerMode;
}

void
gamecontroller_close()
{
	if (controller)
		SDL_GameControllerClose(controller);
	if (haptic)
		SDL_HapticClose(haptic);
}
