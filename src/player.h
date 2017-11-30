#ifndef PLAYER_H_
#define	PLAYER_H_

#include <SDL2/SDL.h>
#include "sprite.h"

enum PlayerClass { ENGINEER, MAGE, PALADIN, ROGUE, WARRIOR };
typedef enum PlayerClass class_t;

Sprite* player_create(class_t, SDL_Renderer*);

#endif // PLAYER_H_
