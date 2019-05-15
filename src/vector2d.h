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

#ifndef VECTOR2D_H_
#define	VECTOR2D_H_

#include <stdbool.h>

#define VECTOR2D_NODIR	(Vector2d) {  0,  0 }
#define VECTOR2D_RIGHT	(Vector2d) {  1,  0 }
#define VECTOR2D_LEFT	(Vector2d) { -1,  0 }
#define VECTOR2D_UP		(Vector2d) {  0, -1 }
#define VECTOR2D_DOWN	(Vector2d) {  0,  1 }

#define VEC2D(x, y)		(Vector2d) { x, y }

typedef struct Vector2d_t {
	float x;
	float y;
} Vector2d;

bool
vector2d_equals(Vector2d, Vector2d);

bool
vector2d_is_opposite(Vector2d, Vector2d);

Vector2d
vector2d_to_direction(const Vector2d*);

void
vector2d_reverse(Vector2d*);

#endif // VECTOR2D_H_
