#ifndef MAP_H_
#define	MAP_H_

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "hashtable.h"
#include "linkedlist.h"
#include "sprite.h"
#include "camera.h"
#include "position.h"
#include "timer.h"
#include "defines.h"
#include "monster.h"

typedef struct MapTile_t {
	int textureIndex0;
	int textureIndex1;
	SDL_Rect clip;
	bool collider;
	bool lightsource;
	bool levelExit;
} MapTile;

typedef struct Room_t {
	MapTile* tiles[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	MapTile* secondary_tiles[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	MapTile* decorations[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
} Room;

typedef struct Map_t {
	Room* rooms[MAP_H_ROOM_COUNT][MAP_V_ROOM_COUNT];
	LinkedList *textures;
	Hashtable *monsterTextures;
	LinkedList *monsters;
	LinkedList *items;
	Position currentRoom;
	Timer *renderTimer;
	int level;
} Map;

Map* map_create(void);

int map_add_texture(Map*, const char *path, SDL_Renderer*);

void map_add_tile(Map *map, Position *tile_pos, MapTile*);

void map_add_decoration(Map *map, Position *tile_pos, MapTile*);

Texture* map_add_monster_texture(Map*, const char *path, SDL_Renderer*);

void map_add_monster(Map*, Monster*);

void map_move_monsters(Map*, RoomMatrix*);

void map_clear_dead_monsters(Map*);

void map_clear_collected_items(Map*);

void map_render(Map*, Camera*);

void map_set_current_room(Map*, Position*);

void map_destroy(Map*);

#endif // MAP_H_
