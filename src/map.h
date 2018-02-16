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

#ifndef MAP_H_
#define	MAP_H_

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "hashtable.h"
#include "linkedlist.h"
#include "sprite.h"
#include "camera.h"
#include "position.h"
#include "timer.h"
#include "defines.h"
#include "monster.h"

typedef struct MapTile_t {
	int textureIndex0;
	int textureIndex1;
	SDL_Rect clip;
	bool collider;
	bool lightsource;
	bool levelExit;
} MapTile;

typedef struct Room_t {
	MapTile* tiles[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	MapTile* decorations[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
} Room;

typedef struct Map_t {
	Room* rooms[MAP_H_ROOM_COUNT][MAP_V_ROOM_COUNT];
	LinkedList *textures;
	Hashtable *monsterTextures;
	LinkedList *monsters;
	LinkedList *items;
	Position currentRoom;
	Timer *renderTimer;
	int level;
} Map;

Map* map_create(void);

int map_add_texture(Map*, const char *path, SDL_Renderer*);

void map_add_tile(Map *map, Position *tile_pos, MapTile*);

void map_add_decoration(Map *map, Position *tile_pos, MapTile*);

Texture* map_add_monster_texture(Map*, const char *path, SDL_Renderer*);

void map_add_monster(Map*, Monster*);

void map_move_monsters(Map*, RoomMatrix*);

void map_clear_dead_monsters(Map*);

void map_clear_collected_items(Map*);

void map_render(Map*, Camera*);

void map_set_current_room(Map*, Position*);

void map_destroy(Map*);

#endif // MAP_H_
