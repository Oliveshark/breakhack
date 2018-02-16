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

#ifndef POSITION_H_
#define	POSITION_H_

#include <stdbool.h>

typedef struct {
	int x;
	int y;
} Position;

Position position_to_matrix_coords(Position*);

Position position_to_room_coords(Position*);

bool position_in_room(Position *pos, Position *roomPos);

bool position_equals(const Position*, const Position*);

#endif // POSITION_H_
