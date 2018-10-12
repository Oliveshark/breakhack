#pragma once

#include <SDL.h>

typedef struct GameController {
	SDL_GameController *controller;
	unsigned int mode;
} GameController;

void
gamecontroller_set(SDL_GameController *controller);

void
gamecontroller_rumble(float intensity, Uint32 duration);

Uint8
gamecontroller_mode(void);

void
gamecontroller_close(void);
