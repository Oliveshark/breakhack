#include "keyboard.h"
#include "util.h"

bool
keyboard_direction_press(Direction dir, SDL_Event *event)
{
	if (event->type != SDL_KEYDOWN)
		return false;

	Uint32 key = event->key.keysym.sym;
	switch (dir) {
		case UP:
			return key == SDLK_UP
				|| key == SDLK_w
				|| key == SDLK_k;
		case DOWN:
			return key == SDLK_DOWN
				|| key == SDLK_s
				|| key == SDLK_j;
		case LEFT:
			return key == SDLK_LEFT
				|| key == SDLK_a
				|| key == SDLK_h;
		case RIGHT:
			return key == SDLK_RIGHT
				|| key == SDLK_l
				|| key == SDLK_d;
		default:
			return false;
	}
}

bool
keyboard_press(Uint32 key, SDL_Event *event)
{
	if (event->type != SDL_KEYDOWN)
		return false;

	return key == (Uint32) event->key.keysym.sym;
}

bool
keyboard_mod_press(Uint32 key, Uint32 mod, SDL_Event *event)
{
	if (event->type != SDL_KEYDOWN)
		return false;

	if (!(event->key.keysym.mod & mod))
		return false;

	return key == (Uint32) event->key.keysym.sym;
}

