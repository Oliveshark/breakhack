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
	map->monsterTextures = ht_create(100);
	map->monsters = linkedlist_create();
	map->currentRoom = (Position) { 0, 0 };
	map->renderTimer = timer_create();
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

Texture*
map_add_monster_texture(Map *map, const char *path, SDL_Renderer *renderer)
{
	Texture *t;

	t = ht_get(map->monsterTextures, path);
	if (!t) {
		t = texture_create();
		texture_load_from_file(t, path, renderer);
		ht_set(map->monsterTextures, path, t);
	}

	return t;
}

void
map_clear_dead_monsters(Map *map)
{
	LinkedList *last, *current, *next;

	last = NULL;
	current = map->monsters;

	while (current != NULL) {
		if (((Monster*) current->data)->stats.hp <= 0) {
			if (last == NULL)
				map->monsters = current->next;
			else
				last->next = current->next;

			monster_destroy(current->data);
			current->data = NULL;
			next = current->next;
			current->next = NULL;
			linkedlist_destroy(&current);
			current = next;
			continue;
		}
		last = current;
		current = current->next;
	}
}

void
map_add_monster(Map *map, Monster *m)
{
	linkedlist_append(&map->monsters, m);
}

void
map_move_monsters(Map *map, RoomMatrix *rm)
{
	LinkedList *m = map->monsters;
	while (m) {
		Monster *monster = m->data;
		m = m->next;
		if (!position_in_room(&monster->sprite->pos, &map->currentRoom))
			continue;
		monster_move(monster, rm);
	}
}

int map_add_texture(Map *map, const char *path, SDL_Renderer *renderer)
{
	Texture *t = texture_create();
	texture_load_from_file(t, path, renderer);
	linkedlist_append(&map->textures, t);
	return linkedlist_size(map->textures) - 1;
}

static
void map_tile_render(Map *map, MapTile *tile, Position *pos, Camera *cam)
{
	static bool second_texture = false;

	if (tile == NULL)
		return;

	if (timer_get_ticks(map->renderTimer) > 300) {
		timer_start(map->renderTimer);
		second_texture = !second_texture;
	}

	Position camPos = camera_to_camera_position(cam, pos);
	SDL_Rect draw_box = (SDL_Rect) {
		camPos.x,
		camPos.y,
		TILE_DIMENSION,
		TILE_DIMENSION
	};

	Texture *texture;
	if (tile->textureIndex1 >= 0 && second_texture) {
		texture = linkedlist_get(&map->textures, tile->textureIndex1);
	} else {
		texture = linkedlist_get(&map->textures, tile->textureIndex0);
	}

	SDL_RenderCopy(cam->renderer,
		       texture->texture,
		       &tile->clip,
		       &draw_box
		      );

}

void map_render(Map *map, Camera *cam)
{
	unsigned int i, j;
	LinkedList *monsterItem;
	Room *room;

	if (!timer_started(map->renderTimer)) {
		timer_start(map->renderTimer);
	}

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
			map_tile_render(map,
					room->decorations[i][j],
					&tilePos,
					cam);
		}
	}

	monsterItem = map->monsters;
	while (monsterItem) {
		Monster *monster = monsterItem->data;
		monsterItem = monsterItem->next;
		monster_render(monster, cam);
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
		texture_destroy(linkedlist_pop(&map->textures));
	}
	while (map->monsters != NULL) {
		monster_destroy(linkedlist_pop(&map->monsters));
	}
	ht_destroy_custom(map->monsterTextures, (void (*)(void*)) texture_destroy);
	timer_destroy(map->renderTimer);
	free(map);
}
