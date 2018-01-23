#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "map_lua.h"
#include "util.h"

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
	bool collider, lightsource, levelExit;

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

	t_index0 = (int) luaL_checkinteger(L, -7);
	t_index1 = (int) luaL_checkinteger(L, -6);
	tile_clip_x = (int) luaL_checkinteger(L, -5);
	tile_clip_y = (int) luaL_checkinteger(L, -4);
	collider = lua_toboolean(L, -3);
	lightsource = lua_toboolean(L, -2);
	levelExit = lua_toboolean(L, -1);

	// Clear the stack
	lua_pop(L, 6);

	Position tilePos = (Position) { tile_x, tile_y };
	SDL_Rect clip = (SDL_Rect) { tile_clip_x, tile_clip_y, 16, 16 };

	MapTile *tile = malloc(sizeof(MapTile));
	*tile = (MapTile) { t_index0,
		t_index1,
		clip, collider,
		lightsource,
		levelExit
	};

	f_add_tile(map, &tilePos, tile);
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
	lua_getfield(L, 4, "clipX");
	lua_getfield(L, 4, "clipY");
	lua_getfield(L, 4, "nstate");
	lua_getfield(L, 4, "cstate");

	tmp_label = luaL_checkstring(L, -7);
	texture_path_1 = luaL_checkstring(L, -6);
	texture_path_2 = luaL_checkstring(L, -5);
	clip_x = (int) luaL_checkinteger(L, -4);
	clip_y = (int) luaL_checkinteger(L, -3);
	nstate = (int) luaL_checkinteger(L, -2);
	cstate = (int) luaL_checkinteger(L, -1);

	texture1 = map_add_monster_texture(map, texture_path_1, renderer);
	texture2 = map_add_monster_texture(map, texture_path_2, renderer);

	label = strdup(tmp_label);

	texture1->dim = (Dimension) { TILE_DIMENSION, TILE_DIMENSION };
	texture2->dim = (Dimension) { TILE_DIMENSION, TILE_DIMENSION };

	lua_pop(L, 6);

	monster = monster_create(renderer);
	monster->sprite->clip = (SDL_Rect) { clip_x, clip_y, 16, 16 };
	monster_update_pos(monster, (Position) { x, y });
	sprite_set_texture(monster->sprite, texture1, 0);
	sprite_set_texture(monster->sprite, texture2, 1);
	monster->state.normal = nstate;
	monster->state.challenge = cstate;
	monster->state.current = nstate;
	if (strlen(label))
		monster->label = label;

	map_add_monster(map, monster);

	return 0;
}

Map* map_lua_generator_run(unsigned int level, SDL_Renderer *renderer)
{
	int status, result;
	char file[] = "data/mapgen.lua";

	printf("[**] Running lua map script: %s\n", file);

	lua_State *L = load_lua_state();
	status = luaL_loadfile(L, file);
	if (status) {
		fprintf(stderr, "[!!] Couldn't load file: %s\n",
			lua_tostring(L, -1));
		exit(-1);
	}

	// Present stuff to lua
	lua_pushlightuserdata(L, renderer);
	lua_setglobal(L, "_sdl_renderer");

	lua_pushcfunction(L, l_create_map);
	lua_setglobal(L, "create_map");

	lua_pushcfunction(L, l_add_tile);
	lua_setglobal(L, "add_tile");

	lua_pushcfunction(L, l_add_decoration);
	lua_setglobal(L, "add_decoration");

	lua_pushcfunction(L, l_add_texture);
	lua_setglobal(L, "add_texture");

	lua_pushcfunction(L, l_map_set_current_room);
	lua_setglobal(L, "set_current_room");

	lua_pushcfunction(L, l_add_monster);
	lua_setglobal(L, "add_monster");

	lua_pushinteger(L, level);
	lua_setglobal(L, "CURRENT_LEVEL");

	result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result) {
		fprintf(stderr, "[!!] Failed to run script: %s\n",
			lua_tostring(L, -1));
		exit(-1);
	}

	lua_getglobal(L, "map");
	Map *map = lua_touserdata(L, 1);

	lua_close(L);

	printf("[**] Done\n");

	return map;
}
