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

#ifndef MAP_LUA_H_
#define	MAP_LUA_H_

#include "map.h"
#include "defines.h"

Map* map_lua_generator_run(unsigned int level, GameMode gameMode, Player *player, SDL_Renderer *renderer);
Map* map_lua_generator_single_room__run(unsigned int level, SDL_Renderer *renderer);

#endif // MAP_LUA_H_
