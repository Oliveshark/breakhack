/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ROOMMATRIX_H_
#define	ROOMMATRIX_H_

#include <stdbool.h>
#include "defines.h"
#include "position.h"
#include "camera.h"
#include "map_room_modifiers.h"
#include "input.h"

typedef struct Sprite Sprite;
typedef struct Map_t Map;
typedef struct Monster Monster;
typedef struct Player Player;
typedef struct Item_t Item;
typedef struct Node LinkedList;
typedef struct Trap Trap;
typedef struct Object Object;

struct UpdateData;

typedef struct RoomSpace {
	bool occupied;
	bool lethal;
	bool lightsource;
	bool damaging;
	int light;
	Monster *monster;
	Player *player;
	Trap *trap;
	LinkedList *items;
	LinkedList *artifacts;
	LinkedList *objects;
} RoomSpace;

typedef struct RoomMatrix_t {
	RoomSpace spaces[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	Position roomPos;
	Position playerRoomPos;
	Position mousePos;
	RoomModifierData *modifier;
} RoomMatrix;

RoomMatrix* roommatrix_create(void);

void
roommatrix_update(struct UpdateData*);

void
roommatrix_populate_from_map(RoomMatrix*, Map*);

void
roommatrix_add_lightsource(RoomMatrix*, Position*);

void
roommatrix_build_lightmap(RoomMatrix*);

void
roommatrix_render_mouse_square(RoomMatrix*, Camera*);

void
roommatrix_render_lightmap(RoomMatrix*, Camera*);

RoomSpace*
roommatrix_get_space_for(RoomMatrix*, const Position *p);

#ifdef DEBUG
void
roommatrix_render_debug(RoomMatrix*, Camera*);
#endif

void
roommatrix_destroy(RoomMatrix*);

Player *
roommatrix_get_player(RoomMatrix*);

#endif // ROOMMATRIX_H_
