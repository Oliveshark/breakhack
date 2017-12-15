#ifndef MONSTER_H_
#define	MONSTER_H_

#include <SDL2/SDL.h>
#include "sprite.h"
#include "stats.h"
#include "actiontext.h"

typedef struct Monster_t {
	Sprite *sprite;
	ActionText *hitText;
	ActionText *missText;
	Stats stats;
} Monster;

Monster* monster_create(SDL_Renderer*);

void monster_update_pos(Monster*, Position);

void monster_render(Monster*, Camera*);

void monster_destroy(Monster*);

#endif // MONSTER_H_
