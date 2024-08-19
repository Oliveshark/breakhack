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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <physfs.h>

#include "map_lua.h"
#include "util.h"
#include "stats.h"
#include "io_util.h"
#include "texturecache.h"
#include "trap.h"
#include "item.h"
#include "item_builder.h"
#include "random.h"
#include "bh_random.h"
#include "artifact.h"

/* Lua registry key variables */
static char RendererKey = 'r';
static char PlayerKey = 'p';

static
lua_State* load_lua_state(void)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	return L;
}

static
int l_create_map(lua_State *L)
{
	Map *map = map_create();
	map->level = (int) luaL_checkinteger(L, 1);
	lua_pushlightuserdata(L, map);
	return 1;
}

static int
l_print_info(lua_State *L)
{
	UNUSED(L); // Compilers keep warning about L being unused
	debug(luaL_checkstring(L, 1));
	return 0;
}

static
Map* luaL_checkmap(lua_State *L, int index)
{
	Map *map;

	if (!lua_islightuserdata(L, index))
		fatal("in luaL_checkmap(), pointer lost in lua script");

	map = lua_touserdata(L, index);
	if (map == NULL)
		fatal("in luaL_checkmap(), bad map pointer");

	return map;
}

static
SDL_Renderer* luaL_checksdlrenderer(lua_State *L)
{
	SDL_Renderer *renderer;

	lua_pushlightuserdata(L, &RendererKey);
	lua_gettable(L, LUA_REGISTRYINDEX);
	if (!lua_islightuserdata(L, -1))
		fatal("in luaL_checksdlrenderer(), pointer lost in lua script");

	renderer = lua_touserdata(L, -1);
	if (renderer == NULL)
		fatal("in luaL_checksdlrenderer(), bad SDL_Renderer pointer");

	return renderer;
}

static Player*
luaL_checkplayer(lua_State *L)
{
	Player *player;
	lua_pushlightuserdata(L, &PlayerKey);
	lua_gettable(L, LUA_REGISTRYINDEX);
	if (!lua_islightuserdata(L, -1))
		fatal("in luaL_checkplayer(), pointer lost in lua script");

	player = lua_touserdata(L, -1);
	return player;
}

static int
l_map_set_current_room_modifier(lua_State *L)
{
	const char *modifier, *direction;

	Map *map = luaL_checkmap(L, 1);
	modifier = luaL_checkstring(L, 2);
	direction = luaL_checkstring(L, 3);

	Vector2d dir = VECTOR2D_LEFT;
	if (strcmp(direction, "LEFT") == 0)
		dir = VECTOR2D_LEFT;
	else if (strcmp(direction, "RIGHT") == 0)
		dir = VECTOR2D_RIGHT;
	else if (strcmp(direction, "UP") == 0)
		dir = VECTOR2D_UP;
	else if (strcmp(direction, "DOWN") == 0)
		dir = VECTOR2D_DOWN;

	if (strcmp(modifier, "WINDY") == 0) {
		Room *room = map->rooms[map->currentRoom.x][map->currentRoom.y];
		room->modifier.type = RMOD_TYPE_WINDY;
		room->modifier.data.wind.direction = dir;
	} else if (strcmp(modifier, "FIRE") == 0) {
		Room *room = map->rooms[map->currentRoom.x][map->currentRoom.y];
		room->modifier.type = RMOD_TYPE_FIRE;
	} else if (strcmp(modifier, "CRUMBLING") == 0) {
		Room *room = map->rooms[map->currentRoom.x][map->currentRoom.y];
		room->modifier.type = RMOD_TYPE_CRUMBLING;
	} else {
		luaL_error(L, "Unknown room modifier: %s", modifier);
		return 1;
	}
	return 0;
}

static int
l_map_set_current_room(lua_State *L)
{
	Map *map;
	unsigned int room_x, room_y;

	map = luaL_checkmap(L, 1);
	room_x = (int) luaL_checkinteger(L, 2);
	room_y = (int) luaL_checkinteger(L, 3);

	map->currentRoom = (Position) { room_x, room_y };

	return 0;
}

static int
l_add_texture(lua_State *L)
{
	Map *map;
	const char *path;
	int index;

	map = luaL_checkmap(L, 1);
	path = luaL_checkstring(L, 2);
	SDL_Renderer *renderer = luaL_checksdlrenderer(L);

	index = map_add_texture(map, path, renderer);
	lua_pushinteger(L, index);
	return 1;
}

static void
extract_tile_data(lua_State *L,
		  void (*f_add_tile)(Map*, Position*, MapTile*))
{
	Map *map;
	int tile_x, tile_y;
	int t_index0, t_index1, tile_clip_x, tile_clip_y;
	bool collider, lightsource, levelExit, lethal;
	int lockType;

	map = luaL_checkmap(L, 1);
	tile_x = (int) luaL_checkinteger(L, 2);
	tile_y = (int) luaL_checkinteger(L, 3);

	// Read the table
	lua_settop(L, 4);
	luaL_checktype(L, 4, LUA_TTABLE);

	// Get the fields from the table
	lua_getfield(L, 4, "textureIndex0");
	lua_getfield(L, 4, "textureIndex1");
	lua_getfield(L, 4, "tileClipX");
	lua_getfield(L, 4, "tileClipY");
	lua_getfield(L, 4, "isCollider");
	lua_getfield(L, 4, "isLightSource");
	lua_getfield(L, 4, "isLevelExit");
	lua_getfield(L, 4, "isLethal");
	lua_getfield(L, 4, "lockType");

	t_index0 = (int) luaL_checkinteger(L, -9);
	t_index1 = (int) luaL_checkinteger(L, -8);
	tile_clip_x = (int) luaL_checkinteger(L, -7);
	tile_clip_y = (int) luaL_checkinteger(L, -6);
	collider = lua_toboolean(L, -5);
	lightsource = lua_toboolean(L, -4);
	levelExit = lua_toboolean(L, -3);
	lethal = lua_toboolean(L, -2);
	lockType = (int) luaL_checkinteger(L, -1);

	// Clear the stack
	lua_pop(L, 9);

	Position tilePos = (Position) { tile_x, tile_y };
	SDL_Rect clip = (SDL_Rect) { tile_clip_x, tile_clip_y, 16, 16 };

	MapTile *tile = map_create_tile();
	if (t_index0 >= 0)
		sprite_set_texture(tile->sprite, linkedlist_get(&map->textures, t_index0), 0);
	if (t_index1 >= 0)
		sprite_set_texture(tile->sprite, linkedlist_get(&map->textures, t_index1), 1);
	tile->sprite->clip = clip;

	tile->collider = collider;
	tile->lightsource = lightsource;
	tile->levelExit = levelExit;
	tile->lethal = lethal;
	tile->lockType = lockType;

	f_add_tile(map, &tilePos, tile);
}

static Stats
lua_checkstats(lua_State *L, int index)
{
	// Confirm table
	if (!lua_istable(L, index)) {
		fatal("Bad table provided");
	}

	// Push to top of stack
	lua_pushvalue(L, index);
	// Stack: -1 => table

	int tableIndex = lua_gettop(L);

	lua_getfield(L, tableIndex, "hp");
	lua_getfield(L, tableIndex, "dmg");
	lua_getfield(L, tableIndex, "atk");
	lua_getfield(L, tableIndex, "def");
	lua_getfield(L, tableIndex, "speed");

	int hp = (int) luaL_checkinteger(L, -5);
	int dmg = (int) luaL_checkinteger(L, -4);
	int atk = (int) luaL_checkinteger(L, -3);
	int def = (int) luaL_checkinteger(L, -2);
	int speed = (int) luaL_checkinteger(L, -1);

	// Reset the stack
	lua_pop(L, 6);

	Stats stats = { hp, hp, dmg, atk, def, speed, 1, false, false };
	return stats;
}

static int
l_add_tile(lua_State *L)
{
	extract_tile_data(L, &map_add_tile);
	return 0;
}

static int
l_add_wall(lua_State *L)
{
	extract_tile_data(L, &map_add_wall);
	return 0;
}

static int
l_add_door(lua_State *L)
{
	extract_tile_data(L, &map_add_door);
	return 0;
}

static int
l_add_decoration(lua_State *L)
{
	extract_tile_data(L, &map_add_decoration);
	return 0;
}

static int
l_add_trap(lua_State *L)
{
	Map *map = luaL_checkmap(L, 1);
	int xpos = (int) luaL_checkinteger(L, 2);
	int ypos = (int) luaL_checkinteger(L, 3);

	// Read the table
	lua_settop(L, 4);
	luaL_checktype(L, 4, LUA_TTABLE);

	// Get the fields from the table
	lua_getfield(L, 4, "texturePath1");
	lua_getfield(L, 4, "texturePath2");
	lua_getfield(L, 4, "clipX");
	lua_getfield(L, 4, "clipY");
	lua_getfield(L, 4, "damage");

	const char *texturePath1 = luaL_checkstring(L, -5);
	const char *texturePath2 = luaL_checkstring(L, -4);
	int clipx = (int) luaL_checkinteger(L, -3);
	int clipy = (int) luaL_checkinteger(L, -2);
	int damage = (int) luaL_checkinteger(L, -1);

	Texture *t0 = texturecache_add(texturePath1);
	Texture *t1 = texturecache_add(texturePath2);

	lua_pop(L, 5);

	const Position *cr = &map->currentRoom;

	Trap *trap = trap_create();
	sprite_set_texture(trap->sprite, t0, 0);
	sprite_set_texture(trap->sprite, t1, 1);
	trap->sprite->clip = CLIP16(clipx, clipy);
	trap->sprite->pos = (Position) {
		cr->x * MAP_ROOM_WIDTH * TILE_DIMENSION + xpos * TILE_DIMENSION,
		cr->y * MAP_ROOM_HEIGHT * TILE_DIMENSION + ypos * TILE_DIMENSION
	};
	trap->damage = damage;

	Position trapPos = { xpos, ypos };
	map_add_trap(map, &trapPos, trap);

	return 0;
}

static int
l_add_chest(lua_State *L)
{
	Map *map = luaL_checkmap(L, 1);
	int x = (int) luaL_checkinteger(L, 2);
	int y = (int) luaL_checkinteger(L, 3);
	int level = (int) luaL_checkinteger(L, 4);

	// Read the table
	lua_settop(L, 5);
	luaL_checktype(L, 5, LUA_TTABLE);

	lua_getfield(L, 5, "texturePath1");
	lua_getfield(L, 5, "texturePath2");
	lua_getfield(L, 5, "clipX");
	lua_getfield(L, 5, "clipY");

	const char *texture_path_1 = luaL_checkstring(L, -4);
	const char *texture_path_2 = luaL_checkstring(L, -3);
	int clip_x = (int) luaL_checkinteger(L, -2);
	int clip_y = (int) luaL_checkinteger(L, -1);

	Item *chest = item_builder_build_container(texture_path_1,
						   texture_path_2,
						   CLIP16(clip_x, clip_y));
	const Position *cr = &map->currentRoom;
	chest->sprite->pos = (Position) {
		cr->x * MAP_ROOM_WIDTH * TILE_DIMENSION + x * TILE_DIMENSION,
		cr->y * MAP_ROOM_HEIGHT * TILE_DIMENSION + y * TILE_DIMENSION
	};
	lua_pop(L, 4);

	if (get_random(1) == 0)
		linkedlist_append(&chest->items, item_builder_build_item(TREASURE, level));
	if (get_random(4) == 0)
		linkedlist_append(&chest->items, item_builder_build_item(HEALTH, level));
	if (get_random(4) == 0) {
		Item *dagger = item_builder_build_item(DAGGER, level);
		dagger->value = get_random(4) + 1;
		linkedlist_append(&chest->items, dagger);
	}

	linkedlist_append(&map->items, chest);

	return 0;
}

static bool
monster_is_in_room_with_locktype(Map *map, Monster *m, DoorLockType lockType)
{
	Position roomPos = position_to_room_coords(&m->sprite->pos);
	Room *room = map->rooms[roomPos.x][roomPos.y];
	return room->lockTypes & lockType;
}

static void
add_keybearer_to_map(Map *map, int keyType)
{
	for (size_t tries = 0; tries < 10; tries++) {
		unsigned int max = linkedlist_size(map->monsters);
		unsigned int index = bh_map_rand() % max;

		Monster *m = linkedlist_get(&map->monsters, index);
		if (!m)
			continue;

		if (m->items.keyType != LOCK_NONE)
			continue;

		if (!monster_is_in_room_with_locktype(map, m, keyType)) {
			debug("Adding key %d to monster '%s' (%u)", keyType, m->label, index);
			m->items.keyType = keyType;
			return;
		} else {
			debug("Looking for another monster to be the keybearer");
		}
	}

	error("Failed to find a suitable keybearer (%d)", keyType);
}

static int
l_add_key_to_random_monsters(lua_State *L)
{
	Map *map = luaL_checkmap(L, 1);
	if (map->lockTypes & LOCK_GOLD)
		add_keybearer_to_map(map, LOCK_GOLD);
	if (map->lockTypes & LOCK_SILVER)
		add_keybearer_to_map(map, LOCK_SILVER);

	return 0;
}

static int
l_add_monster(lua_State *L)
{
	Monster *monster;
	Map *map;
	int x, y, clip_x, clip_y, behaviour;
	const char *texture_path_1, *texture_path_2, *tmp_label;
	char *label;
	bool boss;
	Texture *texture1, *texture2;
	Stats stats;

	map = luaL_checkmap(L, 1);
	x = (int) luaL_checkinteger(L, 2);
	y = (int) luaL_checkinteger(L, 3);

	// Read the table
	lua_settop(L, 4);
	luaL_checktype(L, 4, LUA_TTABLE);

	lua_getfield(L, 4, "label");
	lua_getfield(L, 4, "texturePath1");
	lua_getfield(L, 4, "texturePath2");
	lua_getfield(L, 4, "stats");
	lua_getfield(L, 4, "clipX");
	lua_getfield(L, 4, "clipY");
	lua_getfield(L, 4, "behaviour");
	lua_getfield(L, 4, "boss");

	tmp_label = luaL_checkstring(L, -8);
	texture_path_1 = luaL_checkstring(L, -7);
	texture_path_2 = luaL_checkstring(L, -6);
	stats = lua_checkstats(L, -5);
	clip_x = (int) luaL_checkinteger(L, -4);
	clip_y = (int) luaL_checkinteger(L, -3);
	behaviour = (int) luaL_checkinteger(L, -2);
	boss = (bool) lua_toboolean(L, -1);

	texture1 = texturecache_add(texture_path_1);
	texture2 = texturecache_add(texture_path_2);

	label = strdup(tmp_label);

	lua_pop(L, 8);

	// Make sure traders stay hostile if you've killed one
	monster = monster_create();
	monster->sprite->clip = (SDL_Rect) { clip_x, clip_y, 16, 16 };
	monster_update_pos(monster, (Position) { x, y });
	sprite_set_texture(monster->sprite, texture1, 0);
	sprite_set_texture(monster->sprite, texture2, 1);
	monster_set_behaviour(monster, behaviour);
	if (strlen(label)) {
		monster->label = label;
		monster->lclabel = to_lower(label);
	}

	monster->stats = stats;
	monster->boss = boss;

	map_add_monster(map, monster);

	return 0;
}

static int
l_load_script(lua_State *L)
{
	unsigned long size;
	const char *name = luaL_checkstring(L, 1);
	char *content;

	char filename[20];

	m_sprintf(filename, 20, "%s.lua", name);

	if (!PHYSFS_exists(filename)) {
		luaL_error(L, "Unable to locate module: %s\n", name);
		return 1; // Unable to locate file
	}
	debug("Loading module: %s from %s", name, filename);

	io_load_file_buffer(&content, &size, filename);
	if (luaL_loadbuffer(L, content, size, name) != 0) {
		luaL_error(L, "Error loading module %s from file %s\n\t%s", lua_tostring(L, 1), filename, lua_tostring(L, -1));
	}
	free(content);

	return 1;
}

static int
l_read_file(lua_State *L)
{
	unsigned long size;
	const char *filename = luaL_checkstring(L, 1);
	char *content;

	if (!PHYSFS_exists(filename)) {
		luaL_error(L, "Unable to locate file: %s\n", filename);
		return 1; // Unable to locate file
	}

	io_load_file_buffer(&content, &size, filename);
	lua_pushstring(L, content);
	free(content);

	return 1;
}

static int
l_get_random_seed(lua_State *L)
{
	unsigned int level = (unsigned int) luaL_checkinteger(L, 1);
	lua_pushnumber(L, get_random_map_seed(level));
	return 1;
}

static int
l_set_random_seed(lua_State *L)
{
	unsigned int seed = (unsigned int) luaL_checkinteger(L, 1);
	bh_map_srand(seed);
	return 0;
}

static int
l_get_random(lua_State *L)
{
	unsigned int max = (unsigned int) luaL_checkinteger(L, 1);
	lua_pushnumber(L, (bh_map_rand() % max) + 1);
	return 1;
}

static int
l_create_shop_artifact(lua_State *L)
{
	Player *player = luaL_checkplayer(L);

	if (player == NULL)
		return 0;

	Map *map = luaL_checkmap(L, 1);
	int x = (int) luaL_checkinteger(L, 2);
	int y = (int) luaL_checkinteger(L, 3);

	Artifact *a = artifact_create_random(player, 2);
	a->sprite->pos = POS(x, y);
	artifact_add_price(a, 100);

	linkedlist_append(&map->artifacts, a);

	return 0;
}

static void
build_player_state_table(lua_State *L, Player *player)
{
	lua_createtable(L, 0, 1);
	lua_pushboolean(L, player ? player->stateData.shopOwnerKiller : false);
	lua_setfield(L, -2, "shopOwnerKiller");
	lua_pushnumber(L, player ? player->stats.lvl : 1);
	lua_setfield(L, -2, "level");
	lua_setglobal(L, "PlayerData");
}

static Map*
generate_map(unsigned int level, const char *file, GameMode gameMode, Player *player, SDL_Renderer *renderer)
{
	int status, result;

	debug("Running lua map script: %s", file);

	lua_State *L = load_lua_state();

	char *buffer = NULL;
	long unsigned int len;
	io_load_file_buffer(&buffer, &len, file);
	status = luaL_loadbuffer(L, buffer, len, file);
	free(buffer);

	if (status) {
		fatal("Couldn't load file: %s\n", lua_tostring(L, -1));
	}

	// Present stuff to lua
	build_player_state_table(L, player);

    /* Register state variables in the registry. We use static variable
     * addresses as keys to avoid key conflicts. */
	lua_pushlightuserdata(L, &RendererKey);
	lua_pushlightuserdata(L, renderer);
	lua_settable(L, LUA_REGISTRYINDEX);

	lua_pushlightuserdata(L, &PlayerKey);
	lua_pushlightuserdata(L, player);
	lua_settable(L, LUA_REGISTRYINDEX);

	lua_pushcfunction(L, l_create_shop_artifact);
	lua_setglobal(L, "create_shop_artifact");

	lua_pushcfunction(L, l_create_map);
	lua_setglobal(L, "create_map");

	lua_pushcfunction(L, l_print_info);
	lua_setglobal(L, "info");

	lua_pushcfunction(L, l_add_tile);
	lua_setglobal(L, "add_tile");

	lua_pushcfunction(L, l_add_wall);
	lua_setglobal(L, "add_wall");

	lua_pushcfunction(L, l_add_door);
	lua_setglobal(L, "add_door");

	lua_pushcfunction(L, l_add_decoration);
	lua_setglobal(L, "add_decoration");

	lua_pushcfunction(L, l_add_trap);
	lua_setglobal(L, "add_trap");

	lua_pushcfunction(L, l_add_chest);
	lua_setglobal(L, "add_chest");

	lua_pushcfunction(L, l_add_texture);
	lua_setglobal(L, "add_texture");

	lua_pushcfunction(L, l_read_file);
	lua_setglobal(L, "read_file");

	lua_pushcfunction(L, l_map_set_current_room);
	lua_setglobal(L, "set_current_room");

	lua_pushcfunction(L, l_map_set_current_room_modifier);
	lua_setglobal(L, "set_modifier");

	lua_pushcfunction(L, l_add_monster);
	lua_setglobal(L, "add_monster");

	lua_pushcfunction(L, l_add_key_to_random_monsters);
	lua_setglobal(L, "add_keybearers");

	lua_pushcfunction(L, l_get_random_seed);
	lua_setglobal(L, "get_random_seed");

	lua_pushcfunction(L, l_set_random_seed);
	lua_setglobal(L, "map_randomseed");

	lua_pushcfunction(L, l_get_random);
	lua_setglobal(L, "map_random");

	lua_pushinteger(L, level);
	lua_setglobal(L, "CURRENT_LEVEL");

	lua_pushboolean(L, gameMode == QUICK);
	lua_setglobal(L, "QUICK_MODE");

	lua_pushboolean(L, gameMode == ARCADE);
	lua_setglobal(L, "ARCADE_MODE");

	// Add custom searcher
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "searchers");
	lua_pushcfunction(L, l_load_script);
	lua_rawseti(L, -2, 1);
	lua_pop(L, 2);

	result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result) {
		fatal("Failed to run script: %s\n", lua_tostring(L, -1));
	}

	lua_getglobal(L, "map");
	Map *map = lua_touserdata(L, 1);

	lua_close(L);

	// Reset the map
	map->currentRoom = (Position) { 0, 0 };

	debug("Running lua script %s: Done", file);

	return map;
}

Map* map_lua_generator_single_room__run(unsigned int level, SDL_Renderer *renderer)
{
	char file[] = "menumapgen.lua";
	return generate_map(level, file, REGULAR, NULL, renderer);
}

Map* map_lua_generator_run(unsigned int level, GameMode gameMode, Player *player, SDL_Renderer *renderer)
{
	char file[] = "mapgen.lua";
	return generate_map(level, file, gameMode, player, renderer);
}


