#ifndef ROOMMATRIX_H_
#define	ROOMMATRIX_H_

#include <stdbool.h>
#include "defines.h"
#include "position.h"
#include "camera.h"

typedef struct Sprite_t Sprite;
typedef struct Map_t Map;
typedef struct Monster_t Monster;
typedef struct Player_t Player;
typedef struct Item_t Item;

typedef struct {
	bool occupied;
	bool lightsource;
	unsigned int light;
	Monster *monster;
	Player *player;
	Item *item;
} RoomSpace;

typedef struct {
	RoomSpace spaces[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	Position roomPos;
	Position playerRoomPos;
} RoomMatrix;

RoomMatrix* roommatrix_create(void);

void roommatrix_populate_from_map(RoomMatrix*, Map*);

void roommatrix_update_with_player(RoomMatrix*, Player*);

void roommatrix_add_lightsource(RoomMatrix*, Position*);

void roommatrix_build_lightmap(RoomMatrix*);

void roommatrix_render_lightmap(RoomMatrix*, Camera*);

void roommatrix_reset(RoomMatrix*);

void roommatrix_destroy(RoomMatrix*);

#endif // ROOMMATRIX_H_
