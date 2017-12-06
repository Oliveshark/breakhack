#include <stdio.h>
#include <stdbool.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "map_lua.h"
#include "util.h"

static
lua_State* load_lua_state()
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	return L;
}

static
int l_create_map(lua_State *L)
{
	Map *map = map_create();
	lua_pushlightuserdata(L, map);
	return 1;
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

static
int l_map_set_current_room(lua_State *L)
{
	Map *map;
	unsigned int room_x, room_y;

	map = luaL_checkmap(L, 1);
	room_x = luaL_checkinteger(L, 2);
	room_y = luaL_checkinteger(L, 3);
	
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

static
int l_add_tile(lua_State *L)
{
	Map *map;
	int tile_x, tile_y;
	int texture_index, tile_clip_x, tile_clip_y;
	bool collider;

	map = luaL_checkmap(L, 1);
	tile_x = luaL_checkinteger(L, 2);
	tile_y = luaL_checkinteger(L, 3);
	texture_index = luaL_checkinteger(L, 4);
	tile_clip_x = luaL_checkinteger(L, 5);
	tile_clip_y = luaL_checkinteger(L, 6);
	collider = lua_toboolean(L, 7);

	Position tilePos = (Position) { tile_x, tile_y };
	SDL_Rect clip = (SDL_Rect) { tile_clip_x, tile_clip_y, 16, 16 };

	MapTile *tile = malloc(sizeof(MapTile));
	*tile = (MapTile) { texture_index, clip, collider };

	map_add_tile(map, &tilePos, tile);

	return 0;
}

Map* map_lua_generator_run(SDL_Renderer *renderer)
{
	int status, result;
	char file[] = "data/mapgen.lua";

	printf("[**] Running lua script: %s\n", file);

	lua_State *L = load_lua_state();
	status = luaL_loadfile(L, file);
	if (status) {
		fprintf(stderr, "[!!] Couldn't load file: %s\n", lua_tostring(L, -1));
		exit(-1);
	}

	// Present stuff to lua
	lua_pushlightuserdata(L, renderer);
	lua_setglobal(L, "_sdl_renderer");

	lua_pushcfunction(L, l_create_map);
	lua_setglobal(L, "create_map");

	lua_pushcfunction(L, l_add_tile);
	lua_setglobal(L, "add_tile");

	lua_pushcfunction(L, l_add_texture);
	lua_setglobal(L, "add_texture");

	lua_pushcfunction(L, l_map_set_current_room);
	lua_setglobal(L, "set_current_room");

	result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result) {
		fprintf(stderr, "[!!] Failed to run script: %s\n", lua_tostring(L, -1));
		exit(-1);
	}

	lua_getglobal(L, "map");
	Map *map = lua_touserdata(L, 1);

	lua_close(L);

	printf("[**] Done\n");

	return map;
}
