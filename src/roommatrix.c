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

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "defines.h"
#include "roommatrix.h"
#include "util.h"
#include "map.h"
#include "player.h"
#include "item.h"
#include "update_data.h"
#include "defines.h"
#include "trap.h"
#include "object.h"

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
			space->damaging = false;
			space->lightsource = false;
			space->light = 0;
			space->monster = NULL;
			space->player = NULL;
			space->trap = NULL;
			space->tile = NULL;
			space->wall = NULL;
			space->door = NULL;
			space->decoration = NULL;
			while (space->items != NULL)
				linkedlist_pop(&space->items);
			while (space->artifacts != NULL)
				linkedlist_pop(&space->artifacts);
			while (space->objects != NULL)
				linkedlist_pop(&space->objects);
		}
	}
	m->roomPos = (Position) { 0, 0 };
	m->playerRoomPos = (Position) { 1, 1 };
}

RoomMatrix* roommatrix_create(SDL_Renderer *renderer)
{
	int i, j;
	RoomMatrix *m = ec_malloc(sizeof(RoomMatrix));
	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			m->spaces[i][j].items = linkedlist_create();;
			m->spaces[i][j].artifacts = linkedlist_create();;
			m->spaces[i][j].objects = linkedlist_create();;
		}
	}
	roommatrix_reset(m);

	/* Create a lightmap texture */
	Texture *lm = texture_create();
	lm->dim = (Dimension) { MAP_ROOM_WIDTH, MAP_ROOM_HEIGHT };
	texture_create_blank(lm, SDL_TEXTUREACCESS_TARGET, renderer);
	texture_set_scale_mode(lm, SDL_SCALEMODE_LINEAR);
	texture_set_blend_mode(lm, SDL_BLENDMODE_BLEND);
	m->lightmap = lm;

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

	roommatrix_reset(rm);

	rm->roomPos = m->currentRoom;
	r = m->rooms[rm->roomPos.x][rm->roomPos.y];
	rm->modifier = &r->modifier;

	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			RoomSpace *space = &rm->spaces[i][j];
			if (r->tiles[i][j]) {
				space->tile = r->tiles[i][j];
				space->occupied =
					r->tiles[i][j]->collider;
				space->lightsource =
					r->tiles[i][j]->lightsource;
				space->lethal =
					r->tiles[i][j]->lethal;
			}
			if (r->walls[i][j]) {
				space->wall = r->walls[i][j];
				space->occupied =
					r->walls[i][j]->collider;
				space->lightsource =
					r->walls[i][j]->lightsource;
			}
			if (r->doors[i][j]) {
				space->door = r->doors[i][j];
				space->occupied =
					r->doors[i][j]->collider;
				space->lightsource =
					r->doors[i][j]->lightsource;
			}
			if (r->decorations[i][j]) {
				space->decoration = r->decorations[i][j];
				space->occupied |=
					r->decorations[i][j]->collider;
				space->lightsource |=
					r->decorations[i][j]->lightsource;
			}
			space->trap = r->traps[i][j];
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
			.monster = monster;
	}

	LinkedList *items = m->items;
	while (items) {
		item = items->data;
		items = items->next;

		if (!position_in_room(&item->sprite->pos, &m->currentRoom))
			continue;

		position = position_to_matrix_coords(&item->sprite->pos);
		linkedlist_push(&rm->spaces[position.x][position.y].items, item);
	}

	LinkedList *artifacts = m->artifacts;
	while (artifacts) {
		Artifact *a = artifacts->data;
		artifacts = artifacts->next;

		if (!position_in_room(&a->sprite->pos, &m->currentRoom))
			continue;

		position = position_to_matrix_coords(&a->sprite->pos);
		linkedlist_push(&rm->spaces[position.x][position.y].artifacts, a);
	}

	LinkedList *objects = m->objects;
	while (objects) {
		Object *o = objects->data;
		objects = objects->next;

		if (!position_in_room(&o->sprite->pos, &m->currentRoom))
			continue;

		position = position_to_matrix_coords(&o->sprite->pos);
		RoomSpace *space = &rm->spaces[position.x][position.y];
		linkedlist_push(&space->objects, o);
		space->occupied = space->occupied || o->blocking;
		space->damaging = o->damage > 0;
	}
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
	RoomSpace *space;

	space = &matrix->spaces[x][y];
	if (!space->lightsource)
		return;

	space->light = 255;

	int radius = 4;
	int x_max = min(x + radius, MAP_ROOM_WIDTH - 1);
	int x_min = max(x - radius, 0);
	int y_max = min(y + radius, MAP_ROOM_HEIGHT - 1);
	int y_min = max(y - radius, 0);

	for (int i = x_min; i <= x_max; ++i) {
		for (int j = y_min; j <= y_max; ++j) {
			int lightval = matrix->spaces[i][j].light;

			double dx = x-i;
			double dy = y-j;
			double distance = sqrt(dx*dx + dy*dy);
			int distance_modifier = (int) (distance);
			lightval += 255 - distance_modifier * 50;
			lightval = clamp(0, 255, lightval);
			matrix->spaces[i][j].light = lightval;
		}
	}
}

void
roommatrix_build_lightmap(RoomMatrix *matrix, Camera *camera)
{
	int i, j;
	Uint8 light;

	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			set_light_for_tile(matrix, i, j);
		}
	}

	/* Render spaces light value to lightmap texture */
	SDL_SetRenderTarget(camera->renderer, matrix->lightmap->texture);
	SDL_SetRenderDrawColor(camera->renderer, 0, 0, 0, 0);
	SDL_RenderClear(camera->renderer);
	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			light = (Uint8) matrix->spaces[i][j].light;
			SDL_SetRenderDrawColor(camera->renderer, 0, 0, 0, 255-light);
			SDL_RenderPoint(camera->renderer, (float) i, (float) j);
		}
	}
	SDL_RenderPresent(camera->renderer);
	SDL_SetRenderTarget(camera->renderer, NULL);
}

void
roommatrix_render_mouse_square(RoomMatrix *matrix, Camera *cam)
{
	Position mmc = position_to_matrix_coords(&matrix->mousePos);
	SDL_FRect box = (SDL_FRect) {
			(float) mmc.x*TILE_DIMENSION,
			(float) mmc.y*TILE_DIMENSION,
			TILE_DIMENSION,
			TILE_DIMENSION
	};

	SDL_SetRenderDrawColor(cam->renderer, 255, 255, 0, 90);
	SDL_RenderFillRect(cam->renderer, &box);
}

void
roommatrix_render_lightmap(RoomMatrix *matrix, Camera *cam)
{
	SDL_Rect src_rect = { 0, 0, MAP_ROOM_WIDTH, MAP_ROOM_HEIGHT };
	SDL_Rect dst_rect = { 0, 0, GAME_VIEW_WIDTH, GAME_VIEW_HEIGHT };

	texture_render_clip(matrix->lightmap, &dst_rect, &src_rect, cam);
	texture_render(matrix->lightmap, &src_rect, cam);
}

RoomSpace*
roommatrix_get_space_for(RoomMatrix *matrix, const Position *p)
{
	Position roomPos = position_to_matrix_coords(p);
	return &matrix->spaces[roomPos.x][roomPos.y];
}

void roommatrix_destroy(RoomMatrix *m)
{
	// Clear the list but don't destroy the items
	// The items are destroyed in the map destruction
	for (int i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (int j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			RoomSpace *space = &m->spaces[i][j];
			while (space->items)
				linkedlist_pop(&space->items);
			while (space->artifacts)
				linkedlist_pop(&space->artifacts);
			while (space->objects)
				linkedlist_pop(&space->objects);
		}
	}
    texture_destroy(m->lightmap);

	free(m);
}

Player *
roommatrix_get_player(RoomMatrix *rm)
{
	Position *pos = &rm->playerRoomPos;
	return rm->spaces[pos->x][pos->y].player;
}
