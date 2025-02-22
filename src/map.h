/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2025  Linus Probert <linus.probert@gmail.com>
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

#ifndef MAP_H_
#define	MAP_H_

#include <SDL3/SDL.h>
#include <stdbool.h>

#include "linkedlist.h"
#include "sprite.h"
#include "camera.h"
#include "position.h"
#include "timer.h"
#include "defines.h"
#include "monster.h"
#include "player.h"
#include "map_room_modifiers.h"
#include "object.h"
#include "doorlocktype.h"

typedef struct UpdateData UpdateData;
typedef struct Trap Trap;

typedef struct MapTile_t {
	Sprite *sprite;
	bool collider;
	bool lethal;
	bool lightsource;
	bool levelExit;
	bool door;
	DoorLockType lockType;
} MapTile;

typedef struct Room_t {
	MapTile* tiles[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	MapTile* walls[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	MapTile* decorations[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	MapTile* doors[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	Trap* traps[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	RoomModifierData modifier;
	bool visited;
	unsigned int lockTypes;
} Room;

typedef struct Map_t {
	Room* rooms[MAP_H_ROOM_COUNT][MAP_V_ROOM_COUNT];
	LinkedList *textures;
	LinkedList *monsters;
	LinkedList *items;
	LinkedList *artifacts;
	LinkedList *objects;
	Position currentRoom;
	Timer *monsterMoveTimer;
	int level;
	unsigned int lockTypes;
} Map;

Map*
map_create(void);

MapTile *
map_create_tile(void);

int
map_add_texture(Map*, const char *path, SDL_Renderer*);

void
map_add_tile(Map *map, Position *tile_pos, MapTile*);

void
map_add_wall(Map *map, Position *tile_pos, MapTile*);

void
map_add_decoration(Map *map, Position *tile_pos, MapTile*);

void
map_add_door(Map *map, Position *tile_pos, MapTile*);

void
map_add_trap(Map*, Position*, Trap*);

void
map_add_monster(Map*, Monster*);

bool
map_move_monsters(Map*, RoomMatrix*);

bool
map_clear_expired_entities(Map*, Player*);

void
map_on_new_turn(Map*);

void
map_update(UpdateData*);

void
map_render(Map*, Camera*);

void
map_render_mid_layer(Map*, Camera*);

void
map_render_top_layer(Map*, RoomMatrix*, Camera*);

void
map_set_current_room(Map*, Position*);

void
map_trigger_tile_fall(MapTile *tile);

bool
map_open_door(MapTile *tile, Player *player);

void
map_destroy(Map*);

#endif // MAP_H_
