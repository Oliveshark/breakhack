#ifndef PLAYER_H_
#define	PLAYER_H_

#include <SDL2/SDL.h>
#include "sprite.h"
#include "stats.h"
#include "actiontext.h"
#include "camera.h"

enum PlayerClass { ENGINEER, MAGE, PALADIN, ROGUE, WARRIOR };
typedef enum PlayerClass class_t;

typedef struct Player_t {
	Sprite *sprite;
	ActionText *hitText;
	ActionText *missText;
	Stats stats;
	unsigned int xp;
	unsigned int total_steps;
	unsigned int steps;
	void (*handle_event)(struct Player_t*, RoomMatrix*, SDL_Event*);
} Player;

Player* player_create(class_t, SDL_Renderer*);

void player_hit(Player*, unsigned int dmg);

void player_reset_steps(Player*);

void player_render(Player*, Camera*);

void player_destroy(Player*);

#endif // PLAYER_H_
