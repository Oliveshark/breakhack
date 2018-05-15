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

#ifndef ACTIONTEXT_H_
#define	ACTIONTEXT_H_

#include <SDL.h>
#include <stdbool.h>

#include "position.h"
#include "sprite.h"
#include "timer.h"
#include "vector2d.h"

struct UpdateData_t;

typedef struct {
	Position pos;
	Sprite *sprite;
	bool dead;
	Timer *timer;
	Vector2d velocity;
	SDL_Color color;
} ActionText;

ActionText* actiontext_create(Sprite*);

void actiontext_update(ActionText*, struct UpdateData_t*);

void actiontext_render(ActionText*, Camera*);

void actiontext_destroy(ActionText*);

#endif // ACTIONTEXT_H_
