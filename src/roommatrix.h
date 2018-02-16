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

typedef struct Sprite_t Sprite;
typedef struct Map_t Map;
typedef struct Monster_t Monster;
typedef struct Player_t Player;
typedef struct Item_t Item;
typedef struct Node LinkedList;

typedef struct {
	bool occupied;
	bool lightsource;
	unsigned int light;
	Monster *monster;
	Player *player;
	LinkedList *items;
} RoomSpace;

typedef struct {
	RoomSpace spaces[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	Position roomPos;
	Position playerRoomPos;
	Position mousePos;
} RoomMatrix;

RoomMatrix* roommatrix_create(void);

void roommatrix_handle_event(RoomMatrix*, SDL_Event*);

void roommatrix_populate_from_map(RoomMatrix*, Map*);

void roommatrix_update_with_player(RoomMatrix*, Player*);

void roommatrix_add_lightsource(RoomMatrix*, Position*);

void roommatrix_build_lightmap(RoomMatrix*);

void roommatrix_render_mouse_square(RoomMatrix*, Camera*);

void roommatrix_render_lightmap(RoomMatrix*, Camera*);

void roommatrix_reset(RoomMatrix*);

void roommatrix_destroy(RoomMatrix*);

#endif // ROOMMATRIX_H_
