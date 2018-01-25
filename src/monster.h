#ifndef MONSTER_H_
#define	MONSTER_H_

#include <SDL2/SDL.h>
#include "sprite.h"
#include "stats.h"
#include "actiontext.h"

typedef enum { PASSIVE, AGRESSIVE, SCARED } StateType;

typedef struct {
	StateType current;
	StateType normal;
	StateType challenge;
} State;

typedef struct Monster_t {
	char *label;
	char *lclabel;
	Sprite *sprite;
	ActionText *hitText;
	ActionText *missText;
	Stats stats;
	State state;
} Monster;

Monster* monster_create(SDL_Renderer*);

void monster_update_pos(Monster*, Position);

void monster_move(Monster*, RoomMatrix*);

void monster_render(Monster*, Camera*);

void monster_hit(Monster*, unsigned int dmg);

Item *monster_drop_loot(Monster*);

void monster_destroy(Monster*);

#endif // MONSTER_H_
