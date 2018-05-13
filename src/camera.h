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

#ifndef CAMERA_H_
#define	CAMERA_H_

#include <SDL.h>

#include "position.h"
#include "timer.h"
#include "vector2d.h"

typedef struct {
	Position pos;
	Position basePos;
	Vector2d velocity;
	Timer *shakeTimer;
	SDL_Renderer *renderer;
} Camera;

Camera*
camera_create(SDL_Renderer*);

Position
camera_to_camera_position(Camera *cam, Position *pos);

void
camera_follow_position(Camera*, Position*);

void
camera_update(Camera *, float deltatime);

void
camera_shake(Vector2d dir, int intensity);

void
camera_destroy(Camera *);

#endif // CAMERA_H_
