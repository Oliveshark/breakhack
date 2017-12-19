#include <stdlib.h>
#include "defines.h"
#include "roommatrix.h"
#include "util.h"
#include "map.h"
#include "player.h"

RoomMatrix* roommatrix_create()
{
	RoomMatrix *m = ec_malloc(sizeof(RoomMatrix));
	roommatrix_reset(m);
	return m;
}

void roommatrix_populate_from_map(RoomMatrix *rm, Map *m)
{
	int i, j;
	Position monster_matrix_pos;
	Room *r;
	Monster *monster;
	LinkedList *monsterItem;

	roommatrix_reset(rm);

	rm->roomPos = m->currentRoom;
	r = m->rooms[rm->roomPos.x][rm->roomPos.y];

	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			if (r->tiles[i][j]) {
				rm->spaces[i][j].occupied =
					r->tiles[i][j]->collider;

				rm->spaces[i][j].lightsource =
					r->tiles[i][j]->lightsource;
			}
			if (r->decorations[i][j]) {
				rm->spaces[i][j].occupied |=
					r->decorations[i][j]->collider;

				rm->spaces[i][j].lightsource |=
					r->decorations[i][j]->lightsource;
			}
		}
	}

	monsterItem = m->monsters;
	while (monsterItem) {
		monster = monsterItem->data;
		monsterItem = monsterItem->next;

		if (!position_in_room(&monster->sprite->pos, &m->currentRoom))
			continue;

		monster_matrix_pos =
			position_to_matrix_coords(&monster->sprite->pos);

		rm->spaces[monster_matrix_pos.x][monster_matrix_pos.y]
			.occupied = true;
		rm->spaces[monster_matrix_pos.x][monster_matrix_pos.y]
			.monster = monster;
	}
}

#ifndef _WIN32
static int
min(int a, int b)
{
	return a > b ? b : a;
}

static int
max(int a, int b)
{
	return a > b ? a : b;
}
#endif // _WIN32

void
roommatrix_update_with_player(RoomMatrix *rm, Player *p)
{
	Position rp = position_to_matrix_coords(&p->sprite->pos);
	rm->spaces[rp.x][rp.y].occupied = true;
	rm->spaces[rp.x][rp.y].player = p;
	rm->playerRoomPos = rp;
}

void
roommatrix_add_lightsource(RoomMatrix *matrix, Position *pos)
{
	Position mpos = position_to_matrix_coords(pos);
	matrix->spaces[mpos.x][mpos.y].lightsource = true;
}

static void
set_light_for_tile(RoomMatrix *matrix, int x, int y)
{
	int x_max, x_min, y_max, y_min, i, j;
	int lightval, distance_modifier;
	RoomSpace *space;

	space = &matrix->spaces[x][y];
	if (!space->lightsource)
		return;

	space->light = 255;

	x_max = min(x + 5, MAP_ROOM_WIDTH - 1);
	x_min = max(x - 5, 0);
	y_max = min(y + 5, MAP_ROOM_HEIGHT - 1);
	y_min = max(y - 5, 0);

	for (i = x_min; i <= x_max; ++i) {
		for (j = y_min; j <= y_max; ++j) {
			lightval = matrix->spaces[i][j].light;
			distance_modifier = abs(x-i) == abs(y-j) ?
				abs(x-i) + 1 : max(abs(x-i), abs(y-j));
			lightval += 255 - (distance_modifier * 50);
			lightval = min(255, lightval);
			lightval = max(0, lightval);
			matrix->spaces[i][j].light = lightval;
		}
	}
}

void
roommatrix_build_lightmap(RoomMatrix *matrix)
{
	int i, j;

	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			set_light_for_tile(matrix, i, j);
		}
	}
}

void
roommatrix_render_lightmap(RoomMatrix *matrix, Camera *cam)
{
	int i, j, light;

	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			light = max(0, 230 - matrix->spaces[i][j].light);
			SDL_SetRenderDrawColor(cam->renderer,
					       0, 0, 0, light);
			SDL_Rect box = (SDL_Rect) {
				i*TILE_DIMENSION,
				j*TILE_DIMENSION,
				TILE_DIMENSION,
				TILE_DIMENSION
			};
			SDL_RenderFillRect(cam->renderer, &box);
		}
	}
}

void roommatrix_reset(RoomMatrix *m)
{
	int i, j;

	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			m->spaces[i][j].occupied = false;
			m->spaces[i][j].lightsource = false;
			m->spaces[i][j].light = 0;
			m->spaces[i][j].monster = NULL;
			m->spaces[i][j].player = NULL;
		}
	}
	m->roomPos = (Position) { 0, 0 };
	m->playerRoomPos = (Position) { 1, 1 };
}

void roommatrix_destroy(RoomMatrix *m)
{
	free(m);
}
