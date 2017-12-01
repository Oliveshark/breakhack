#include "map.h"
#include "util.h"

static
Sprite* create_default_tile(SDL_Renderer *renderer)
{
	Sprite *s = sprite_create();
	sprite_load_texture(s, "assets/Objects/Floor.png", renderer);
	s->texture->clip = (SDL_Rect) { 16, 54, 16, 16 };
	s->texture->dim = (Dimension) { 64, 64 };
	return s;
}

static
Room* create_room()
{
	int i, j;
	Room *room;

	room = ec_malloc(sizeof(Room));
	for (i=0; i < MAP_ROOM_HEIGHT; ++i) {
		for (j=0; j < MAP_ROOM_WIDTH; ++j) {
			room->tiles[i][j] = NULL;
		}
	}
	return room;
}

Map* map_create(SDL_Renderer *renderer)
{
	int i, j;

	Map *map = ec_malloc(sizeof(Map));
	map->currentRoom = (Position) { 0, 0 };
	map->level = 1;
	map->defaultTile = create_default_tile(renderer);
	
	for (i=0; i < MAP_V_ROOM_COUNT; ++i) {
		for (j=0; j < MAP_H_ROOM_COUNT; ++j) {
			map->rooms[i][j] = create_room();
		}
	}

	return map;
}

void map_render(Map *map, Camera *cam)
{
	int i, j;
	Room *room;
	Position roomPos = { map->currentRoom.x, map->currentRoom.y };
	Position roomCords = {
		roomPos.x * MAP_ROOM_WIDTH * 64,
		roomPos.y * MAP_ROOM_HEIGHT * 64
	};
	Sprite *dTile = map->defaultTile;

	room = map->rooms[roomPos.x][roomPos.y];
	for (i=0; i < MAP_ROOM_HEIGHT; ++i) {
		for (j=0; j < MAP_ROOM_WIDTH; ++j) {
			if (room->tiles[i][j] == NULL) {
				dTile->pos.x = roomCords.x + (64*j);
				dTile->pos.y = roomCords.y + (64*i);
				sprite_render(dTile, cam);
			}
		}
	}
}

static
void map_room_destroy(Room *room)
{
	int i, j;
	for (i=0; i < MAP_ROOM_HEIGHT; ++i) {
		for (j=0; j < MAP_ROOM_WIDTH; ++j) {
			if (room->tiles[i][j]) {
				sprite_destroy(room->tiles[i][j]);
			}
		}
	}
	free(room);
}

void map_destroy(Map *map)
{
	int i, j;
	for (i=0; i < MAP_V_ROOM_COUNT; ++i) {
		for (j=0; j < MAP_H_ROOM_COUNT; ++j) {
			map_room_destroy(map->rooms[i][j]);
		}
	}
	free(map);
}
