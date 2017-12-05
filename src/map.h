#ifndef MAP_H_
#define	MAP_H_

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <linkedlist.h>

#include "sprite.h"
#include "camera.h"
#include "position.h"

#define MAP_ROOM_HEIGHT 12
#define MAP_ROOM_WIDTH	16
#define MAP_V_ROOM_COUNT 10
#define MAP_H_ROOM_COUNT 10
#define TILE_DIMENSION 64

typedef struct {
	unsigned int textureIndex;
	SDL_Rect clip;
	bool collider;
} MapTile;

typedef struct {
	MapTile* tiles[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
} Room;

typedef struct {
	Room* rooms[MAP_H_ROOM_COUNT][MAP_V_ROOM_COUNT];
	LinkedList *textures;
	Position currentRoom;
	int level;
} Map;

Map* map_create();

int map_add_texture(Map*, const char *path, SDL_Renderer*);

void map_add_tile(Map *map, Position *tile_pos, MapTile*);

void map_render(Map*, Camera*);

void map_set_current_room(Map*, Position*);

void map_destroy(Map*);

#endif // MAP_H_
