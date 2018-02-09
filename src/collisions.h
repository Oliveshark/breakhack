#ifndef COLLISIONS_H_
#define	COLLISIONS_H_

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "position.h"

bool
position_in_rect(Position*, SDL_Rect*);

#endif // COLLISIONS_H_
