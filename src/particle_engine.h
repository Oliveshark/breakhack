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

#ifndef PARTICLE_ENGINE_H_
#define	PARTICLE_ENGINE_H_

#include <SDL.h>
#include "position.h"
#include "dimension.h"
#include "camera.h"
#include "vector2d.h"

void
particle_engine_init(void);

void
particle_engine_bloodspray(Position, Dimension, unsigned int count);

void
particle_engine_fire_explosion(Position, Dimension);

void
particle_engine_eldritch_explosion(Position, Dimension);

void
particle_engine_speed_lines(Position, Dimension, bool horizontal);

void
particle_engine_sparkle(Position, Dimension);

void
particle_engine_wind(Vector2d direction);

void
particle_engine_update(float deltatime);

void
particle_engine_render(Camera*);

void
particle_engine_clear(void);

void
particle_engine_close(void);

#endif // PARTICLE_ENGINE_H_
