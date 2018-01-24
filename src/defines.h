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

#define RIGHT_GUI_WIDTH		(10 * SPRITE_DIMENSION)
#define RIGHT_GUI_HEIGHT	GAME_VIEW_HEIGHT

#define BOTTOM_GUI_HEIGHT	(10 * SPRITE_DIMENSION)
#define BOTTOM_GUI_WIDTH	(GAME_VIEW_WIDTH + RIGHT_GUI_WIDTH)

#define SCREEN_WIDTH		(GAME_VIEW_WIDTH + RIGHT_GUI_WIDTH)
#define SCREEN_HEIGHT		(GAME_VIEW_HEIGHT + BOTTOM_GUI_HEIGHT)

/* Windows and compile crap */
#ifdef _WIN32
#define strdup _strdup
#endif // _WIN32

#define UNUSED(x) (void)(x)

#endif // DEFINES_H_
