#include "map.h"
#include "map_lua.h"
#include "util.h"

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

Map* map_create()
{
	int i, j;

	Map *map = ec_malloc(sizeof(Map));
	map->textures = linkedlist_create();
	map->currentRoom = (Position) { 0, 0 };
	map->level = 1;
	
	for (i=0; i < MAP_V_ROOM_COUNT; ++i) {
		for (j=0; j < MAP_H_ROOM_COUNT; ++j) {
			map->rooms[i][j] = create_room();
		}
	}

	return map;
}

void map_add_tile(Map *map, Position *room_pos, Position *tile_pos, MapTile *tile)
{
	MapTile **oldTile = &map->rooms[room_pos->x][room_pos->y]->tiles[tile_pos->x][tile_pos->y];
	if (*oldTile != NULL) {
		free(*oldTile);
		*oldTile = NULL;
	}
	*oldTile = tile;
}

int map_add_texture(Map *map, const char *path, SDL_Renderer *renderer)
{
	Texture *t = texture_create(path, renderer);
	linkedlist_append(&map->textures, t, sizeof(*t));
	return linkedlist_size(map->textures) - 1;
}

static
void map_tile_render(Map *map, MapTile *tile, Position *pos, Camera *cam)
{
	if (tile == NULL)
		return;

	Position camPos = camera_to_camera_position(cam, pos);
	SDL_Rect draw_box = (SDL_Rect) {
		camPos.x,
		camPos.y,
		64,
		64
	};

	Texture *texture = linkedlist_get(&map->textures, tile->textureIndex);

	SDL_RenderCopy(cam->renderer,
		       texture->texture,
		       &tile->clip,
		       &draw_box
		      );

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

	room = map->rooms[roomPos.x][roomPos.y];
	for (i=0; i < MAP_ROOM_HEIGHT; ++i) {
		for (j=0; j < MAP_ROOM_WIDTH; ++j) {
			Position tilePos = (Position) {
				roomCords.x + j*64,
				roomCords.y + i*64
			};
			map_tile_render(map, room->tiles[i][j], &tilePos, cam);
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
				free(room->tiles[i][j]);
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
	while (map->textures != NULL) {
		SDL_DestroyTexture(linkedlist_poplast(&map->textures));
	}
	free(map);
}
