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

#include "position.h"
#include "defines.h"

Position position_to_matrix_coords(Position *src)
{
	unsigned int room_px_width, room_px_height;
	Position pos;

	room_px_width = TILE_DIMENSION * MAP_ROOM_WIDTH;
	room_px_height = TILE_DIMENSION * MAP_ROOM_HEIGHT;

	pos.x = (src->x % room_px_width) / TILE_DIMENSION;
	pos.y = (src->y % room_px_height) / TILE_DIMENSION;

	return pos;
}

Position position_to_room_coords(Position *src)
{
	unsigned int room_px_width, room_px_height;
	Position pos;

	room_px_width = TILE_DIMENSION * MAP_ROOM_WIDTH;
	room_px_height = TILE_DIMENSION * MAP_ROOM_HEIGHT;

	pos.x = (src->x - (src->x % room_px_width)) / room_px_width;
	pos.y = (src->y - (src->y % room_px_height)) / room_px_height;

	return pos;
}

bool
position_equals(const Position *p1, const Position *p2)
{
	return p1->x == p2->x && p1->y == p2->y;
}

bool position_in_room(Position *pos, Position *roomPos)
{
	int room_px_width, room_px_height, room_x_px, room_y_px;

	room_px_width = TILE_DIMENSION * MAP_ROOM_WIDTH;
	room_px_height = TILE_DIMENSION * MAP_ROOM_HEIGHT;

	room_x_px = roomPos->x * room_px_width;
	room_y_px = roomPos->y * room_px_height;

	if (pos->x < room_x_px)
		return false;
	else if (pos->x > room_x_px + room_px_width)
		return false;
	else if (pos->y < room_y_px)
		return false;
	else if (pos->y > room_y_px + room_px_height)
		return false;

	return true;
}
