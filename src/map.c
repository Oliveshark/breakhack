#include "map.h"
#include "map_lua.h"
#include "util.h"

static
Room* create_room()
{
	int i, j;
	Room *room;

	room = ec_malloc(sizeof(Room));
	for (i=0; i < MAP_ROOM_WIDTH; ++i) {
		for (j=0; j < MAP_ROOM_HEIGHT; ++j) {
			room->tiles[i][j] = NULL;
			room->decorations[i][j] = NULL;
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
	
	for (i=0; i < MAP_H_ROOM_COUNT; ++i) {
		for (j=0; j < MAP_V_ROOM_COUNT; ++j) {
			map->rooms[i][j] = create_room();
		}
	}

	return map;
}

void map_add_tile(Map *map, Position *tile_pos, MapTile *tile)
{
	const Position *cr = &map->currentRoom;
	MapTile **oldTile = &map->rooms[cr->x][cr->y]->tiles[tile_pos->x][tile_pos->y];
	if (*oldTile != NULL) {
		free(*oldTile);
		*oldTile = NULL;
	}
	*oldTile = tile;
}

void map_add_decoration(Map *map, Position *tile_pos, MapTile *tile)
{
	const Position *cr = &map->currentRoom;
	MapTile **oldTile = &map->rooms[cr->x][cr->y]->decorations[tile_pos->x][tile_pos->y];
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

	/* Freeing the texture preserves the underlying SDL_Texture* which
	 * isn't duplicated when it's being added to the list.
	 * texture_destroy() would destroy that too and break rendering.
	 * Unstable solution. Might cause problems if Texture is ever extended
	 * with more data.
	 */
	free(t);
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
		TILE_DIMENSION,
		TILE_DIMENSION
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
		roomPos.x * MAP_ROOM_WIDTH * TILE_DIMENSION,
		roomPos.y * MAP_ROOM_HEIGHT * TILE_DIMENSION
	};

	room = map->rooms[roomPos.x][roomPos.y];
	for (i=0; i < MAP_ROOM_WIDTH; ++i) {
		for (j=0; j < MAP_ROOM_HEIGHT; ++j) {
			Position tilePos = (Position) {
				roomCords.x + i*TILE_DIMENSION,
				roomCords.y + j*TILE_DIMENSION
			};
			map_tile_render(map, room->tiles[i][j], &tilePos, cam);
			map_tile_render(map, room->decorations[i][j], &tilePos, cam);
		}
	}
}

void map_set_current_room(Map *map, Position *pos)
{
	unsigned int room_width, room_height;

	room_width = MAP_ROOM_WIDTH * TILE_DIMENSION;
	room_height = MAP_ROOM_HEIGHT * TILE_DIMENSION;

	if (pos->x <= 0) {
		map->currentRoom.x = 0;
	} else {
		unsigned int room_cord_x = pos->x - (pos->x % room_width);
		map->currentRoom.x = room_cord_x / room_width;
	}

	if (pos->y <= 0) {
		map->currentRoom.y = 0;
	} else {
		unsigned int room_cord_y = pos->y - (pos->y % room_height);
		map->currentRoom.y = room_cord_y / room_height;
	}

	if (map->currentRoom.x >= MAP_H_ROOM_COUNT)
		map->currentRoom.x = MAP_H_ROOM_COUNT - 1;
	if (map->currentRoom.y >= MAP_V_ROOM_COUNT)
		map->currentRoom.y = MAP_V_ROOM_COUNT - 1;
}

static
void map_room_destroy(Room *room)
{
	int i, j;
	for (i=0; i < MAP_ROOM_WIDTH; ++i) {
		for (j=0; j < MAP_ROOM_HEIGHT; ++j) {
			if (room->tiles[i][j]) {
				free(room->tiles[i][j]);
			}
			if (room->decorations[i][j]) {
				free(room->decorations[i][j]);
			}
		}
	}
	free(room);
}

void map_destroy(Map *map)
{
	int i, j;
	for (i=0; i < MAP_H_ROOM_COUNT; ++i) {
		for (j=0; j < MAP_V_ROOM_COUNT; ++j) {
			map_room_destroy(map->rooms[i][j]);
		}
	}
	while (map->textures != NULL) {
		texture_destroy(linkedlist_poplast(&map->textures));
	}
	free(map);
}
