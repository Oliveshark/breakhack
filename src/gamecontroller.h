#pragma once

#include <SDL.h>

typedef struct GameController {
	SDL_GameController *controller;
	unsigned int mode;
} GameController;

void
gamecontroller_set(SDL_GameController *controller);

Uint8
gamecontroller_mode(void);

void
gamecontroller_close(void);
