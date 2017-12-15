#ifndef PLAYER_H_
#define	PLAYER_H_

#include <SDL2/SDL.h>
#include "sprite.h"
#include "stats.h"

enum PlayerClass { ENGINEER, MAGE, PALADIN, ROGUE, WARRIOR };
typedef enum PlayerClass class_t;

typedef struct Player_t {
	Sprite *sprite;
	Stats stats;
	void (*handle_event)(struct Player_t*, RoomMatrix*, SDL_Event*);
} Player;

Player* player_create(class_t, SDL_Renderer*);

void player_destroy(Player*);

#endif // PLAYER_H_
