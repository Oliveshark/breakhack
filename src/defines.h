#ifndef DEFINES_H_
#define	DEFINES_H_

#include "config.h"

/* Room/Map dimensions */
#define MAP_ROOM_WIDTH		16
#define MAP_ROOM_HEIGHT		12

#define MAP_V_ROOM_COUNT	10
#define MAP_H_ROOM_COUNT	10

#define TILE_DIMENSION		32

/* Display stuff */
#define SCREEN_WIDTH	MAP_ROOM_WIDTH * TILE_DIMENSION
#define SCREEN_HEIGHT	MAP_ROOM_HEIGHT * TILE_DIMENSION

/* Windows and compile crap */
#ifdef _WIN32
#define strdup _strdup
#endif // _WIN32

#define UNUSED(x) (void)(x)

#endif // DEFINES_H_
