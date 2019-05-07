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

#include "timer.h"
#include "position.h"
#include "dimension.h"

typedef struct ParticleEmitter {
	unsigned int timestep;
	Timer *timer;
	Position pos;
	Dimension dim;
	void (*particle_func)(Position, Dimension);
	bool enabled;
} ParticleEmitter;

ParticleEmitter *
particle_emitter_create(void);

void
particle_emitter_render(ParticleEmitter*);

void
particle_emitter_update(ParticleEmitter*, Position, Dimension);

void
particle_emitter_destroy(ParticleEmitter*);
