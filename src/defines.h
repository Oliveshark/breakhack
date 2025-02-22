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

#ifndef DEFINES_H_
#define	DEFINES_H_

#include <stdint.h>
#include "gamecontroller.h"
#include "config.h"

/* Room/Map dimensions */
#define MAP_ROOM_WIDTH		16
#define MAP_ROOM_HEIGHT		12

#define MAP_V_ROOM_COUNT	10
#define MAP_H_ROOM_COUNT	10

#define TILE_DIMENSION		32
#define SPRITE_DIMENSION	16

/* Display stuff */
#define GAME_VIEW_WIDTH		(MAP_ROOM_WIDTH * TILE_DIMENSION) // 16 * 32
#define GAME_VIEW_HEIGHT	(MAP_ROOM_HEIGHT * TILE_DIMENSION) // 12 * 32

#define SKILL_BAR_WIDTH		GAME_VIEW_WIDTH
#define SKILL_BAR_HEIGHT	32

#define RIGHT_GUI_WIDTH		(10 * SPRITE_DIMENSION) // 10 * 16
#define MINIMAP_GUI_HEIGHT	128
#define STATS_GUI_HEIGHT	(GAME_VIEW_HEIGHT + SKILL_BAR_HEIGHT - MINIMAP_GUI_HEIGHT)

#define BOTTOM_GUI_HEIGHT	(10 * SPRITE_DIMENSION)
#define BOTTOM_GUI_WIDTH	(GAME_VIEW_WIDTH + RIGHT_GUI_WIDTH)

#define SCREEN_WIDTH		(GAME_VIEW_WIDTH + RIGHT_GUI_WIDTH)
#define SCREEN_HEIGHT		(GAME_VIEW_HEIGHT + SKILL_BAR_HEIGHT + BOTTOM_GUI_HEIGHT)

/* Quality of life stuff */
#define DEFAULT_DIMENSION	(Dimension) { 16, 16 }
#define GAME_DIMENSION		(Dimension) { TILE_DIMENSION, TILE_DIMENSION }
#define CLIP16(x, y)		(SDL_Rect) { x, y, 16, 16 }
#define CLIP32(x, y)		(SDL_Rect) { x, y, 32, 32 }

/* Windows and compile crap */
#ifdef _WIN32
#define strdup _strdup
#endif // _WIN32

#define UNUSED(x) (void)(x)

#define UNPACK_COLOR(color) color.r, color.g, color.b, color.a
#define C_WHITE (SDL_Color)		{	255, 255, 255, 255 }
#define C_RED (SDL_Color)		{	255,   0,   0, 255 }
#define C_GREEN (SDL_Color)		{	  0, 255,   0, 255 }
#define C_BLUE (SDL_Color)		{	 60, 134, 252, 255 }
#define C_LIGHTBLUE (SDL_Color)	{	 143, 178, 234, 255 }
#define C_YELLOW (SDL_Color)	{	255, 255,   0, 255 }
#define C_BLACK (SDL_Color)		{	  0,   0,   0, 255 }
#define C_PURPLE (SDL_Color)	{	137,  16, 229, 255 }
#define C_GREY (SDL_Color)		{	 89,  89,  89, 255 }

// MSVC seems to have min/max defined.
// Haven't looked into it further.
#ifndef _MSC_VER
#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)
#endif // _MSC_VER
#define clamp(a, b, x) max(a, min(b, x))

typedef int8_t		Sint8;
typedef uint8_t		Uint8;
typedef int16_t		Sint16;
typedef uint16_t	Uint16;
typedef int32_t		Sint32;
typedef uint32_t	Uint32;
typedef int64_t		Sint64;
typedef uint64_t	Uint64;

typedef enum Direction_t {
	UP, DOWN, LEFT, RIGHT, INVALID
} Direction;

typedef enum GameMode {
	REGULAR,
	QUICK,
	ARCADE
} GameMode;

#define CONTROLLER_BTN(xindex, mode) CLIP16(xindex, mode == GAMEPAD_TYPE_XB ? 0 : 16)
#define CONTROLLER_TRIGGER(xindex, mode) CLIP16(xindex + (mode == GAMEPAD_TYPE_XB ? 16 : 0), 32)
#define CONTROLLER_BUMPER(xindex, mode) CLIP16(xindex + (mode == GAMEPAD_TYPE_XB ? 16 : 0), 48)
#define CONTROLLER_OPT(xindex, mode) CLIP16(xindex + (mode == GAMEPAD_TYPE_PS ? 16 : 0), 64)

#endif // DEFINES_H_
