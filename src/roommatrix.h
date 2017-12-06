#ifndef ROOMMATRIX_H_
#define	ROOMMATRIX_H_

#include <stdbool.h>
#include "defines.h"
#include "position.h"

typedef struct Sprite_t Sprite;
typedef struct Map_t Map;

typedef struct {
	bool occupied;
	Sprite* character;
	Sprite* player;
} RoomSpace;

typedef struct {
	RoomSpace spaces[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	Position roomPos;
} RoomMatrix;

RoomMatrix* roommatrix_create();

void roommatrix_populate_from_map(RoomMatrix*, Map*);

void roommatrix_reset(RoomMatrix*);

void roommatrix_destroy(RoomMatrix*);

#endif // ROOMMATRIX_H_
