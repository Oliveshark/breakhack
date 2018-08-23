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
#pragma once

#include <SDL.h>
#include <stdbool.h>
#include "camera.h"
#include "player.h"
#include "sprite.h"
#include "player.h"

typedef struct Object {
	Sprite *sprite;
	bool blocking;
	Uint32 damage;
	Sint32 timeout;
	bool dead;
} Object;

Object *
object_create(void);

Object *
object_create_fire(void);

Object *
object_create_green_gas(void);

void
object_render(Object*, Camera*);

void
object_damage(Object*, Player*);

void
object_step(Object*);

void
object_destroy(Object*);
