#include "gamecontroller.h"
#include "util.h"

static SDL_GameController *controller = NULL;
static Uint8 controllerMode = 0;

void
gamecontroller_set(SDL_GameController *ctrler)
{
	const char *ctrlName = SDL_GameControllerName(controller);
	info("Game controller connected: %s", ctrlName);
	controller = ctrler;

	// Try to determine if this is a PS3/4 controller
	if (ctrlName[0] == 'P' &&
	    ctrlName[1] == 'S' &&
	    (ctrlName[2] == '4' || ctrlName[2] == '3'))
		controllerMode = 2;
	else
		controllerMode = 1;
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
}
