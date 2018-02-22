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

#ifndef DEFINES_H_
#define	DEFINES_H_

#include "config.h"

/* Room/Map dimensions */
#define MAP_ROOM_WIDTH		16
#define MAP_ROOM_HEIGHT		12

#define MAP_V_ROOM_COUNT	10
#define MAP_H_ROOM_COUNT	10

#define TILE_DIMENSION		32
#define SPRITE_DIMENSION	16

/* Display stuff */
#define GAME_VIEW_WIDTH		(MAP_ROOM_WIDTH * TILE_DIMENSION)
#define GAME_VIEW_HEIGHT	(MAP_ROOM_HEIGHT * TILE_DIMENSION)

#define SKILL_BAR_WIDTH		GAME_VIEW_WIDTH
#define SKILL_BAR_HEIGHT	32

#define RIGHT_GUI_WIDTH		(10 * SPRITE_DIMENSION)
#define RIGHT_GUI_HEIGHT	(GAME_VIEW_HEIGHT + SKILL_BAR_HEIGHT)

#define BOTTOM_GUI_HEIGHT	(10 * SPRITE_DIMENSION)
#define BOTTOM_GUI_WIDTH	(GAME_VIEW_WIDTH + RIGHT_GUI_WIDTH)

#define SCREEN_WIDTH		(GAME_VIEW_WIDTH + RIGHT_GUI_WIDTH)
#define SCREEN_HEIGHT		(RIGHT_GUI_HEIGHT + BOTTOM_GUI_HEIGHT)

/* Windows and compile crap */
#ifdef _WIN32
#define strdup _strdup
#endif // _WIN32

#define UNUSED(x) (void)(x)

#endif // DEFINES_H_
