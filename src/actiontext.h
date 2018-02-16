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

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "position.h"
#include "texture.h"
#include "timer.h"

typedef struct {
	Position pos;
	Texture *texture;
	bool active;
	Timer *timer;
	SDL_Color color;
} ActionText;

ActionText* actiontext_create(void);

void actiontext_load_font(ActionText*, const char *path, unsigned int size);

void actiontext_set_text(ActionText*, const char *text, SDL_Renderer*);

void actiontext_render(ActionText*, Camera*);

void actiontext_destroy(ActionText*);

#endif // ACTIONTEXT_H_
