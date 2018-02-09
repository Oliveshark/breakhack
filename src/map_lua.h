#ifndef MAP_LUA_H_
#define	MAP_LUA_H_

#include "map.h"

Map* map_lua_generator_run(unsigned int level, SDL_Renderer *renderer);
Map* map_lua_generator_single_room__run(unsigned int level, SDL_Renderer *renderer);

#endif // MAP_LUA_H_
