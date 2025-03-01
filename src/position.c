/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2025  Linus Probert <linus.probert@gmail.com>
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
#include "position.h"
#include "defines.h"

#define ROOM_PX_WIDTH (TILE_DIMENSION * MAP_ROOM_WIDTH)
#define ROOM_PX_HEIGHT (TILE_DIMENSION * MAP_ROOM_HEIGHT)

Position
position_to_matrix_coords(const Position *src)
{
	Position pos;

	pos.x = (src->x % ROOM_PX_WIDTH) / TILE_DIMENSION;
	pos.y = (src->y % ROOM_PX_HEIGHT) / TILE_DIMENSION;

	return pos;
}

Position
position_to_room_coords(Position *src)
{
	Position pos;

	pos.x = (src->x - (src->x % ROOM_PX_WIDTH)) / ROOM_PX_WIDTH;
	pos.y = (src->y - (src->y % ROOM_PX_HEIGHT)) / ROOM_PX_HEIGHT;

	return pos;
}

bool
position_equals(const Position *p1, const Position *p2)
{
	return p1->x == p2->x && p1->y == p2->y;
}

bool
position_proximity(unsigned int distance,
		   const Position *p1,
		   const Position *p2)
{
	unsigned int dx, dy;

	dx = abs(p1->x - p2->x);
	dy = abs(p1->y - p2->y);

	return dx <= distance && dy <= distance;
}

bool
position_in_room(Position *pos, Position *roomPos)
{
	int room_x_px, room_y_px;

	room_x_px = roomPos->x * ROOM_PX_WIDTH;
	room_y_px = roomPos->y * ROOM_PX_HEIGHT;

	if (pos->x < room_x_px)
		return false;
	else if (pos->x >= room_x_px + ROOM_PX_WIDTH)
		return false;
	else if (pos->y < room_y_px)
		return false;
	else if (pos->y >= room_y_px + ROOM_PX_HEIGHT)
		return false;

	return true;
}

bool
position_in_roommatrix(const Position *pos)
{
	return pos->x >= 0
		&& pos->x < MAP_ROOM_WIDTH
		&& pos->y >= 0
		&& pos->y < MAP_ROOM_HEIGHT;
}

Position
position_to_tile_pos(const Position *p)
{
	return (Position) {
		p->x - (p->x % TILE_DIMENSION),
		p->y - (p->y % TILE_DIMENSION)
	};
}

Position
position_add(const Position *a, const Position *b)
{
	return POS(a->x + b->x, a->y + b->y);
}

Position
position_mcord_to_world_pos(const Position *cord, const Position *roomPos)
{

	return POS(roomPos->x * GAME_VIEW_WIDTH  + cord->x * TILE_DIMENSION,
			roomPos->y * GAME_VIEW_HEIGHT + cord->y * TILE_DIMENSION);
}
