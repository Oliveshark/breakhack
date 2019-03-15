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

#include <stdlib.h>
#include "map.h"
#include "map_lua.h"
#include "util.h"
#include "item.h"
#include "item_builder.h"
#include "gui.h"
#include "particle_engine.h"
#include "update_data.h"
#include "trap.h"

static Room*
create_room(void)
{
	int i, j;
	Room *room;

	room = ec_malloc(sizeof(Room));
	room->modifier.type = RMOD_TYPE_NONE;
	room->visited = false;
	for (i=0; i < MAP_ROOM_WIDTH; ++i) {
		for (j=0; j < MAP_ROOM_HEIGHT; ++j) {
			room->tiles[i][j] = NULL;
			room->walls[i][j] = NULL;
			room->decorations[i][j] = NULL;
			room->traps[i][j] = NULL;
			room->doors[i][j] = NULL;
		}
	}
	return room;
}

Map*
map_create(void)
{
	int i, j;

	Map *map = ec_malloc(sizeof(Map));
	map->textures = linkedlist_create();
	map->monsters = linkedlist_create();
	map->items = linkedlist_create();
	map->artifacts = linkedlist_create();
	map->objects = linkedlist_create();
	map->currentRoom = (Position) { 0, 0 };
	map->monsterMoveTimer = _timer_create();
	map->level = 1;
	
	for (i=0; i < MAP_H_ROOM_COUNT; ++i) {
		for (j=0; j < MAP_V_ROOM_COUNT; ++j) {
			map->rooms[i][j] = create_room();
		}
	}

	return map;
}

MapTile*
map_create_tile(void)
{
	MapTile *tile = ec_malloc(sizeof(MapTile));
	tile->sprite = sprite_create();
	tile->sprite->clip = CLIP16(0, 0);
	tile->sprite->dim = DIM(32, 32);
	tile->collider = false;
	tile->lethal = false;
	tile->lightsource = false;
	tile->levelExit = false;
	tile->door = false;
	tile->lockType = LOCK_NONE;
	return tile;
}

static void
map_tile_destroy(MapTile *tile)
{
	if (tile->sprite)
		sprite_destroy(tile->sprite);
	free(tile);
}

static void
switch_tile(Map *map, Position *tile_pos, MapTile *tile, MapTile **oldTile)
{
	// Set the decoration sprites position to match tile pos
	tile->sprite->pos = POS(tile_pos->x * TILE_DIMENSION + (map->currentRoom.x * GAME_VIEW_WIDTH),
				tile_pos->y * TILE_DIMENSION + (map->currentRoom.y * GAME_VIEW_HEIGHT));

	if (*oldTile != NULL) {
		map_tile_destroy(*oldTile);
		*oldTile = NULL;
	}
	*oldTile = tile;
}

void
map_add_tile(Map *map, Position *tile_pos, MapTile *tile)
{
	const Position *cr = &map->currentRoom;
	Room *room = map->rooms[cr->x][cr->y];
	switch_tile(map, tile_pos, tile, &room->tiles[tile_pos->x][tile_pos->y]);

	// If this is the level exit then clear the decoration if one exists
	if (tile->levelExit && room->decorations[tile_pos->x][tile_pos->y]) {
		MapTile **decoration = &room->decorations[tile_pos->x][tile_pos->y];
		map_tile_destroy(*decoration);
		*decoration = NULL;
	}
}

void
map_add_wall(Map *map, Position *tile_pos, MapTile *tile)
{
	const Position *cr = &map->currentRoom;
	Room *room = map->rooms[cr->x][cr->y];
	switch_tile(map, tile_pos, tile, &room->walls[tile_pos->x][tile_pos->y]);
}

void map_add_decoration(Map *map, Position *tile_pos, MapTile *tile)
{
	const Position *cr = &map->currentRoom;
	Room *room = map->rooms[cr->x][cr->y];
	switch_tile(map, tile_pos, tile, &room->decorations[tile_pos->x][tile_pos->y]);
}

void
map_add_door(Map *map, Position *tile_pos, MapTile *tile)
{
	const Position *cr = &map->currentRoom;
	Room *room = map->rooms[cr->x][cr->y];
	switch_tile(map, tile_pos, tile, &room->doors[tile_pos->x][tile_pos->y]);
	tile->door = true;
	tile->sprite->texture_index = 0;
	tile->sprite->animate = false;
}

void
map_add_trap(Map *map, Position *pos, Trap *trap)
{
	const Position *cr = &map->currentRoom;
	Trap **oldTrap = &map->rooms[cr->x][cr->y]->traps[pos->x][pos->y];
	if (*oldTrap)
		trap_destroy(*oldTrap);

	*oldTrap = trap;
}

bool
map_clear_expired_entities(Map *map, Player *player)
{
	LinkedList *filtered = linkedlist_create();
	bool anythingCleared = false;

	while (map->monsters) {
		Monster *monster = linkedlist_pop(&map->monsters);
		if (monster->stats.hp <= 0) {
			monster_drop_loot(monster, map, player);
			monster_destroy(monster);
			anythingCleared = true;
		} else {
			linkedlist_append(&filtered, monster);
		}
	}
	map->monsters = filtered;

	filtered = linkedlist_create();
	while (map->items) {
		Item *item = linkedlist_pop(&map->items);
		if (item->collected) {
			item_destroy(item);
			anythingCleared = true;
		} else {
			linkedlist_append(&filtered, item);
		}
	}
	map->items = filtered;

	filtered = linkedlist_create();
	while (map->artifacts) {
		Artifact *a = linkedlist_pop(&map->artifacts);
		if (!a->collected) {
			linkedlist_append(&filtered, a);
		} else {
			artifact_destroy(a);
			anythingCleared = true;
		}
	}
	map->artifacts = filtered;

	filtered = linkedlist_create();
	while (map->objects) {
		Object *o = linkedlist_pop(&map->objects);
		if (o->dead) {
			object_destroy(o);
			anythingCleared = true;
		} else {
			linkedlist_append(&filtered, o);
		}
	}
	map->objects = filtered;

	return anythingCleared;
}

void
map_add_monster(Map *map, Monster *m)
{
	monster_update_stats_for_level(m, map->level);
	linkedlist_append(&map->monsters, m);
}

bool
map_move_monsters(Map *map, RoomMatrix *rm)
{
	LinkedList *m = map->monsters;
	bool allDone = true;

	if (timer_started(map->monsterMoveTimer)
	    && timer_get_ticks(map->monsterMoveTimer) < 100)
		return false;

	while (m) {
		Monster *monster = m->data;
		m = m->next;
		if (monster->stats.hp <= 0)
			continue;
		if (!position_in_room(&monster->sprite->pos, &map->currentRoom))
			continue;

		// Prevent passive monsters from being "dodgy"
		Position pos = position_to_matrix_coords(&monster->sprite->pos);
		if (monster->state.current == PASSIVE
		    && position_proximity(1, &rm->playerRoomPos, &pos))
			continue;
		if (monster->steps >= monster->stats.speed)
			continue;

		allDone = allDone && monster_move(monster, rm, map);
	}

	if (allDone) {
		timer_stop(map->monsterMoveTimer);
		linkedlist_each(&map->monsters, (void (*)(void*)) monster_reset_steps);
	} else {
		timer_start(map->monsterMoveTimer);
	}

	return allDone;
}

int map_add_texture(Map *map, const char *path, SDL_Renderer *renderer)
{
	Texture *t = texture_create();
	texture_load_from_file(t, path, renderer);
	linkedlist_append(&map->textures, t);
	return linkedlist_size(map->textures) - 1;
}

static
void map_tile_render(MapTile *tile, Camera *cam)
{
	if (tile == NULL)
		return;

	sprite_render(tile->sprite, cam);
}

void
map_on_new_turn(Map *map)
{
	LinkedList *objects = map->objects;
	while (objects) {
		Object *o = objects->data;
		objects = objects->next;
		object_step(o);
	}
}

void
map_update(UpdateData *data)
{
	Map *map = data->map;
	LinkedList *monster = map->monsters;
	while (monster) {
		Monster *m = monster->data;
		monster = monster->next;
		monster_update(m, data);
	}

	Position roomPos = { map->currentRoom.x, map->currentRoom.y };
	Room *room = map->rooms[roomPos.x][roomPos.y];
	for (size_t i=0; i < MAP_ROOM_WIDTH; ++i) {
		for (size_t j=0; j < MAP_ROOM_HEIGHT; ++j) {
			if (room->tiles[i][j])
				sprite_update(room->tiles[i][j]->sprite, data);
		}
	}

	LinkedList *items = map->items;
	while (items) {
		item_update(items->data);
		items = items->next;
	}
}

void map_render(Map *map, Camera *cam)
{
	unsigned int i, j;

	Position roomPos = { map->currentRoom.x, map->currentRoom.y };
	Room *room = map->rooms[roomPos.x][roomPos.y];
	for (i=0; i < MAP_ROOM_WIDTH; ++i) {
		for (j=0; j < MAP_ROOM_HEIGHT; ++j) {
			map_tile_render(room->tiles[i][j], cam);
			map_tile_render(room->walls[i][j], cam);
			map_tile_render(room->doors[i][j], cam);
			map_tile_render(room->decorations[i][j], cam);

			if (room->traps[i][j])
				trap_render(room->traps[i][j], cam);
		}
	}
	if (room->modifier.type == RMOD_TYPE_WINDY) {
		particle_engine_wind(room->modifier.data.wind.direction);
	} else if (room->modifier.type == RMOD_TYPE_FIRE) {
		particle_engine_heat();
	}

}

void
map_render_mid_layer(Map *map, Camera *cam)
{
	LinkedList *items = map->items;
	while (items != NULL) {
		item_render(items->data, cam);
		items = items->next;
	}

	LinkedList *objects = map->objects;
	while (objects != NULL) {
		object_render(objects->data, cam);
		objects = objects->next;
	}

	LinkedList *artifacts = map->artifacts;
	while (artifacts != NULL) {
		artifact_render(artifacts->data, cam);
		artifacts = artifacts->next;
	}

	LinkedList *monsterItem = map->monsters;
	while (monsterItem != NULL) {
		Monster *monster = monsterItem->data;
		monsterItem = monsterItem->next;
		monster_render(monster, cam);
	}
}

void
map_render_top_layer(Map *map, RoomMatrix *rm, Camera *cam)
{
	LinkedList *monsterItem = map->monsters;
	while (monsterItem != NULL) {
		Monster *monster = monsterItem->data;
		monsterItem = monsterItem->next;
		monster_render_top_layer(monster, rm, cam);
	}
}

void map_set_current_room(Map *map, Position *pos)
{
	unsigned int room_width, room_height;

	room_width = MAP_ROOM_WIDTH * TILE_DIMENSION;
	room_height = MAP_ROOM_HEIGHT * TILE_DIMENSION;

	if (pos->x <= 0) {
		map->currentRoom.x = 0;
	} else {
		unsigned int room_cord_x = pos->x - (pos->x % room_width);
		map->currentRoom.x = room_cord_x / room_width;
	}

	if (pos->y <= 0) {
		map->currentRoom.y = 0;
	} else {
		unsigned int room_cord_y = pos->y - (pos->y % room_height);
		map->currentRoom.y = room_cord_y / room_height;
	}

	if (map->currentRoom.x >= MAP_H_ROOM_COUNT)
		map->currentRoom.x = MAP_H_ROOM_COUNT - 1;
	if (map->currentRoom.y >= MAP_V_ROOM_COUNT)
		map->currentRoom.y = MAP_V_ROOM_COUNT - 1;

	map->rooms[map->currentRoom.x][map->currentRoom.y]->visited = true;
}

static
void map_room_destroy(Room *room)
{
	int i, j;
	for (i=0; i < MAP_ROOM_WIDTH; ++i) {
		for (j=0; j < MAP_ROOM_HEIGHT; ++j) {
			if (room->tiles[i][j]) {
				map_tile_destroy(room->tiles[i][j]);
			}
			if (room->doors[i][j]) {
				map_tile_destroy(room->doors[i][j]);
			}
			if (room->walls[i][j]) {
				map_tile_destroy(room->walls[i][j]);
			}
			if (room->decorations[i][j]) {
				map_tile_destroy(room->decorations[i][j]);
			}
			if (room->traps[i][j]) {
				trap_destroy(room->traps[i][j]);
			}
		}
	}
	free(room);
}

void
map_trigger_tile_fall(MapTile *tile)
{
	if (tile->sprite->state != SPRITE_STATE_FALLING && tile->sprite->state != SPRITE_STATE_PLUMMETED)
		particle_engine_dust_puff(tile->sprite->pos, tile->sprite->dim);

	tile->sprite->state = SPRITE_STATE_FALLING;
	tile->lethal = true;
}

void
map_destroy(Map *map)
{
	int i, j;
	for (i=0; i < MAP_H_ROOM_COUNT; ++i) {
		for (j=0; j < MAP_V_ROOM_COUNT; ++j) {
			map_room_destroy(map->rooms[i][j]);
		}
	}

	while (map->textures != NULL)
		texture_destroy(linkedlist_pop(&map->textures));

	while (map->monsters != NULL)
		monster_destroy(linkedlist_pop(&map->monsters));

	while (map->items != NULL)
		item_destroy(linkedlist_pop(&map->items));

	while (map->artifacts != NULL)
		artifact_destroy(linkedlist_pop(&map->artifacts));

	while (map->objects != NULL)
		object_destroy(linkedlist_pop(&map->objects));

	timer_destroy(map->monsterMoveTimer);
	free(map);
}
