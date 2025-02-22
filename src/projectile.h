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

#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include <stdbool.h>
#include "sprite.h"
#include "camera.h"
#include "vector2d.h"
#include "timer.h"
#include "roommatrix.h"
#include "update_data.h"
#include "defines.h"

#define DAGGER_VELOCITY 500

typedef struct Projectile {
	Sprite *sprite;
	Vector2d velocity;
	Timer *lifetime;
	bool alive;
	Uint32 collisionCount;
	Uint32 bounceCount;
	bool processedSpaces[MAP_ROOM_WIDTH][MAP_ROOM_HEIGHT];
	void (*onRender)(struct Projectile*);
} Projectile;

Projectile *
projectile_dagger_create(void);

Projectile *
projectile_create(void);

void
projectile_update(Projectile*, UpdateData *);

void
projectile_render(Projectile*, Camera*);

void
projectile_destroy(Projectile*);

#endif // PROJECTILE_H_
