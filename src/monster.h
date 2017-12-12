#ifndef MONSTER_H_
#define	MONSTER_H_

#include <SDL2/SDL.h>
#include "sprite.h"
#include "stats.h"

typedef struct {
	Sprite *sprite;
	Stats stats;
} Monster;

#endif // MONSTER_H_
