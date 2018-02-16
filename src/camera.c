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

#include "camera.h"
#include "map.h"

Position camera_to_camera_position(Camera *cam, Position *pos)
{
	return (Position) {
		pos->x - cam->pos.x,
		pos->y - cam->pos.y
	};
}

void camera_follow_position(Camera *cam, Position *pos)
{
	int room_width, room_height;

	room_width = MAP_ROOM_WIDTH * TILE_DIMENSION;
	room_height = MAP_ROOM_HEIGHT * TILE_DIMENSION;

	if (pos->x <= 0)
		cam->pos.x = 0;
	else if (pos->x >= room_width * MAP_H_ROOM_COUNT)
		cam->pos.x = room_width * (MAP_H_ROOM_COUNT - 1);
	else
		cam->pos.x = pos->x - (pos->x % room_width);

	if (pos->y <= 0)
		cam->pos.y = 0;
	else if (pos->y >= room_height * MAP_V_ROOM_COUNT)
		cam->pos.y = room_height * (MAP_V_ROOM_COUNT - 1);
	else
		cam->pos.y = pos->y - (pos->y % room_height);
}
