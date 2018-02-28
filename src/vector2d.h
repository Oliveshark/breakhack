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

#define VECTOR2D_RIGHT	(Vector2d) {  1,  0 }
#define VECTOR2D_LEFT	(Vector2d) { -1,  0 }
#define VECTOR2D_UP		(Vector2d) {  0, -1 }
#define VECTOR2D_DOWN	(Vector2d) {  0,  1 }

typedef struct Vector2d_t {
	float x;
	float y;
} Vector2d;

#endif // VECTOR2D_H_
