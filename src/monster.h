#ifndef MONSTER_H_
#define	MONSTER_H_

#include <SDL2/SDL.h>
#include "sprite.h"
#include "stats.h"

typedef struct {
	Sprite *sprite;
	Stats stats;
	SDL_Rect clip;
} Monster;

Monster* monster_create();

void monster_render(Monster*, Camera*);

void monster_destroy(Monster*);

#endif // MONSTER_H_
