#ifndef KEYBOARD_H_
#define	KEYBOARD_H_

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef enum Direction_t {
	UP, DOWN, LEFT, RIGHT
} Direction;

bool
keyboard_direction_press(Direction, SDL_Event*);

bool
keyboard_press(Uint32 key, SDL_Event*);

bool
keyboard_mod_press(Uint32 key, Uint32 mod, SDL_Event*);

#endif // KEYBOARD_H_
