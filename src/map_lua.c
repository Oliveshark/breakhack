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
	const char *str = luaL_checkstring(L, 1);
	debug(str);
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

	lua_getglobal(L, "_sdl_renderer");
	if (!lua_islightuserdata(L, -1))
		fatal("in luaL_checksdlrenderer(), pointer lost in lua script");

	renderer = lua_touserdata(L, -1);
	if (renderer == NULL)
		fatal("in luaL_checksdlrenderer(), bad SDL_Renderer pointer");

	return renderer;
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
	} else {
		luaL_error(L, "Unknown room modifier: %s", modifier);
		return 1;
	}
	return 0;
}

static
int l_map_set_current_room(lua_State *L)
{
	Map *map;
	unsigned int room_x, room_y;

	map = luaL_checkmap(L, 1);
	room_x = (int) luaL_checkinteger(L, 2);
	room_y = (int) luaL_checkinteger(L, 3);
	
	map->currentRoom = (Position) { room_x, room_y };

	return 0;
}

static
int l_add_texture(lua_State *L)
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

	t_index0 = (int) luaL_checkinteger(L, -8);
	t_index1 = (int) luaL_checkinteger(L, -7);
	tile_clip_x = (int) luaL_checkinteger(L, -6);
	tile_clip_y = (int) luaL_checkinteger(L, -5);
	collider = lua_toboolean(L, -4);
	lightsource = lua_toboolean(L, -3);
	levelExit = lua_toboolean(L, -2);
	lethal = lua_toboolean(L, -1);

	// Clear the stack
	lua_pop(L, 8);

	Position tilePos = (Position) { tile_x, tile_y };
	SDL_Rect clip = (SDL_Rect) { tile_clip_x, tile_clip_y, 16, 16 };

	MapTile *tile = map_create_tile();
	tile->textureIndex0 = t_index0;
	tile->textureIndex1 = t_index1;
	tile->clip = clip;
	tile->collider = collider;
	tile->lightsource = lightsource;
	tile->levelExit = levelExit;
	tile->lethal = lethal;

	f_add_tile(map, &tilePos, tile);
}

static Stats
lua_checkstats(lua_State *L, int index)
{
	// Confirm table
	lua_istable(L, index);

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

	Stats stats = { hp, hp, dmg, atk, def, speed, 1 };
	return stats;
}

static
int l_tile_occupied(lua_State *L)
{
	Map *map;
	Room *room;
	MapTile *tile, *decor;
	Position *rPos;
	int x, y;
	bool response = false;

	map = luaL_checkmap(L, 1);
	x = (int) luaL_checkinteger(L, 2);
	y = (int) luaL_checkinteger(L, 3);

	rPos = &map->currentRoom;
	room = map->rooms[rPos->x][rPos->y];

	tile = room->tiles[x][y];
	decor = room->decorations[x][y];

	response = response || (tile && (tile->collider || tile->levelExit || tile->lethal));
	response = response || (decor && (decor->collider || decor->levelExit));

	lua_pushboolean(L, response);
	return 1;
}

static
int l_add_tile(lua_State *L)
{
	extract_tile_data(L, &map_add_tile);
	return 0;
}

static
int l_add_decoration(lua_State *L)
{
	extract_tile_data(L, &map_add_decoration);
	return 0;
}

static int
l_add_monster(lua_State *L)
{
	Monster *monster;
	Map *map;
	int x, y, clip_x, clip_y, nstate, cstate;
	const char *texture_path_1, *texture_path_2, *tmp_label;
	char *label;
	Texture *texture1, *texture2;
	SDL_Renderer *renderer;
	Stats stats;

	renderer = luaL_checksdlrenderer(L);
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
	lua_getfield(L, 4, "nstate");
	lua_getfield(L, 4, "cstate");

	tmp_label = luaL_checkstring(L, -8);
	texture_path_1 = luaL_checkstring(L, -7);
	texture_path_2 = luaL_checkstring(L, -6);
	stats = lua_checkstats(L, -5);
	clip_x = (int) luaL_checkinteger(L, -4);
	clip_y = (int) luaL_checkinteger(L, -3);
	nstate = (int) luaL_checkinteger(L, -2);
	cstate = (int) luaL_checkinteger(L, -1);

	texture1 = texturecache_add(texture_path_1);
	texture2 = texturecache_add(texture_path_2);

	label = strdup(tmp_label);

	texture1->dim = GAME_DIMENSION;
	texture2->dim = GAME_DIMENSION;

	lua_pop(L, 8);

	monster = monster_create(renderer);
	monster->sprite->clip = (SDL_Rect) { clip_x, clip_y, 16, 16 };
	monster_update_pos(monster, (Position) { x, y });
	sprite_set_texture(monster->sprite, texture1, 0);
	sprite_set_texture(monster->sprite, texture2, 1);
	monster->state.normal = nstate;
	monster->state.challenge = cstate;
	monster->state.current = nstate;
	if (strlen(label)) {
		monster->label = label;
		monster->lclabel = to_lower(label);
	}

	monster->stats = stats;

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

static Map*
generate_map(unsigned int level, const char *file, SDL_Renderer *renderer)
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
	lua_pushlightuserdata(L, renderer);
	lua_setglobal(L, "_sdl_renderer");

	lua_pushcfunction(L, l_create_map);
	lua_setglobal(L, "create_map");

	lua_pushcfunction(L, l_print_info);
	lua_setglobal(L, "info");

	lua_pushcfunction(L, l_add_tile);
	lua_setglobal(L, "add_tile");

	lua_pushcfunction(L, l_add_decoration);
	lua_setglobal(L, "add_decoration");

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

	lua_pushcfunction(L, l_tile_occupied);
	lua_setglobal(L, "tile_occupied");

	lua_pushinteger(L, level);
	lua_setglobal(L, "CURRENT_LEVEL");

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
	return generate_map(level, file, renderer);
}

Map* map_lua_generator_run(unsigned int level, SDL_Renderer *renderer)
{
	char file[] = "mapgen.lua";
	return generate_map(level, file, renderer);
}


