/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <math.h>
#include "defines.h"
#include "roommatrix.h"
#include "util.h"
#include "map.h"
#include "player.h"
#include "item.h"
#include "update_data.h"

static void
roommatrix_reset(RoomMatrix *m)
{
	RoomSpace *space;
	int i, j;

	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			space = &m->spaces[i][j];
			space->occupied = false;
			space->lethal = false;
			space->lightsource = false;
			space->light = 0;
			space->monster = NULL;
			space->player = NULL;
			while (space->items != NULL)
				linkedlist_pop(&space->items);
		}
	}
	m->roomPos = (Position) { 0, 0 };
	m->playerRoomPos = (Position) { 1, 1 };
}

RoomMatrix* roommatrix_create(void)
{
	int i, j;
	RoomMatrix *m = ec_malloc(sizeof(RoomMatrix));
	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			m->spaces[i][j].items = NULL;
		}
	}
	roommatrix_reset(m);
	return m;
}

static void
roommatrix_update_with_player(RoomMatrix *rm, Player *p)
{
	Position rp = position_to_matrix_coords(&p->sprite->pos);
	rm->spaces[rp.x][rp.y].occupied = true;
	rm->spaces[rp.x][rp.y].player = p;
	rm->playerRoomPos = rp;
}

void
roommatrix_update(UpdateData *data)
{
	RoomMatrix *matrix = data->matrix;
	Input *input = data->input;

	if (input->mouseX < GAME_VIEW_WIDTH
	    && input->mouseY < GAME_VIEW_HEIGHT)
	{
		matrix->mousePos.x = input->mouseX;
		matrix->mousePos.y = input->mouseY;
	}
	roommatrix_update_with_player(matrix, data->player);
}

void roommatrix_populate_from_map(RoomMatrix *rm, Map *m)
{
	int i, j;
	Position position;
	Room *r;
	Monster *monster;
	LinkedList *monsterItem;
	Item *item;
	LinkedList *items;

	roommatrix_reset(rm);

	rm->roomPos = m->currentRoom;
	r = m->rooms[rm->roomPos.x][rm->roomPos.y];
	rm->modifier = &r->modifier;

	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			RoomSpace *space = &rm->spaces[i][j];
			if (r->tiles[i][j]) {
				space->occupied =
					r->tiles[i][j]->collider;
				space->lightsource =
					r->tiles[i][j]->lightsource;
				space->lethal =
					r->tiles[i][j]->lethal;
			}
			if (r->decorations[i][j]) {
				space->occupied |=
					r->decorations[i][j]->collider;
				space->lightsource |=
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

		position =
			position_to_matrix_coords(&monster->sprite->pos);

		rm->spaces[position.x][position.y]
			.occupied = true;
		rm->spaces[position.x][position.y]
			.monster = monster;
	}

	items = m->items;
	while (items) {
		item = items->data;
		items = items->next;

		if (!position_in_room(&item->sprite->pos, &m->currentRoom))
			continue;

		position = position_to_matrix_coords(&item->sprite->pos);
		linkedlist_push(&rm->spaces[position.x][position.y].items, item);
	}
}

// TODO(Linus): These should probably be macros
#undef min
#ifndef min
static int
min(int a, int b)
{
	return a > b ? b : a;
}
#endif // min

#undef max
#ifndef max
static int
max(int a, int b)
{
	return a > b ? a : b;
}
#endif // max

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
				min(abs(x-i) + 1, 5) : max(abs(x-i), abs(y-j));
			lightval += (255 - (distance_modifier * 50));
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
roommatrix_render_mouse_square(RoomMatrix *matrix, Camera *cam)
{
	Position mmc = position_to_matrix_coords(&matrix->mousePos);
	SDL_Rect box = (SDL_Rect) {
			mmc.x*TILE_DIMENSION,
			mmc.y*TILE_DIMENSION,
			TILE_DIMENSION,
			TILE_DIMENSION
	};

	SDL_SetRenderDrawColor(cam->renderer, 255, 255, 0, 90);
	SDL_RenderFillRect(cam->renderer, &box);
}

void
roommatrix_render_lightmap(RoomMatrix *matrix, Camera *cam)
{
	int i, j, light;

	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			light = max(245 - matrix->spaces[i][j].light, 0);

			SDL_Rect box = (SDL_Rect) {
				i*TILE_DIMENSION,
				j*TILE_DIMENSION,
				TILE_DIMENSION,
				TILE_DIMENSION
			};

			SDL_SetRenderDrawColor(cam->renderer,
					       0, 0, 0, (Uint8) light);
			SDL_RenderFillRect(cam->renderer, &box);

#ifdef LIGHTMAPDEBUG
			Texture *t = create_light_texture(light, cam);
			Position p = { box.x+3, box.y+3 };
			texture_render(t, &p, cam);
			texture_destroy(t);
#endif // LIGHTMAPDEBUG
		}
	}
}

#ifdef DEBUG
static void
draw_raycasts_from(int x, int y, Camera *cam)
{
	double length = 4*32;
	double x1 = x * 32 + 16;
	double y1 = y * 32 + 16;
	double angle = 0;

	while (angle < 360) {
		double x2 = x1 + length * cos(angle);
		double y2 = y1 + length * sin(angle);
		//printf("Drawing line: %fx%f -> %fx%f\n", x1, y1, x2, y2);
		SDL_RenderDrawLine(cam->renderer,
				   (int) x1,
				   (int) y1,
				   (int) x2,
				   (int) y2);
		angle += 7.5;
	}
}

void
roommatrix_render_debug(RoomMatrix *rm, Camera *cam)
{
	SDL_SetRenderDrawColor(cam->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	for (int i = 0; i < MAP_ROOM_HEIGHT; ++i) {
		SDL_RenderDrawLine(cam->renderer, 0, i*32, MAP_ROOM_WIDTH*32, i*32);
	}
	for (int i = 0; i < MAP_ROOM_WIDTH; ++i) {
		SDL_RenderDrawLine(cam->renderer, i*32, 0, i*32, MAP_ROOM_HEIGHT*32);
	}

	// Draw raycasts
	SDL_SetRenderDrawColor(cam->renderer, 255, 255, 255, 100);
	for (int i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (int j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			if (rm->spaces[i][j].lightsource) {
				draw_raycasts_from(i, j, cam);
			}
		}
	}
}
#endif

void roommatrix_destroy(RoomMatrix *m)
{
	// Clear the list but don't destroy the items
	// The items are destroyed in the map destruction
	for (int i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (int j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			RoomSpace *space = &m->spaces[i][j];
			while (space->items)
				linkedlist_pop(&space->items);
		}
	}

	free(m);
}
