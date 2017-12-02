#ifndef MAP_H_
#define	MAP_H_

#include <SDL2/SDL.h>
#include <linkedlist.h>

#include "sprite.h"
#include "camera.h"
#include "position.h"

#define MAP_ROOM_HEIGHT 12
#define MAP_ROOM_WIDTH	16
#define MAP_V_ROOM_COUNT 10
#define MAP_H_ROOM_COUNT 10

typedef struct {
	unsigned int textureIndex;
	SDL_Rect clip;
} MapTile;

typedef struct {
	MapTile* tiles[MAP_ROOM_HEIGHT][MAP_ROOM_WIDTH];
} Room;

typedef struct {
	Room* rooms[MAP_V_ROOM_COUNT][MAP_H_ROOM_COUNT];
	LinkedList *textures;
	Position currentRoom;
	int level;
} Map;

Map* map_create();

int map_add_texture(Map*, const char *path, SDL_Renderer*);

void map_add_tile(Map *map, Position *room_pos, Position *tile_pos, MapTile*);

void map_render(Map*, Camera*);

void map_destroy(Map*);

#endif // MAP_H_
