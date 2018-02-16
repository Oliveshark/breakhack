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

#ifndef TEXTURE_H_
#define	TEXTURE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "dimension.h"
#include "position.h"
#include "camera.h"

typedef struct {
	SDL_Texture *texture;
	TTF_Font *font;
	Dimension dim;
} Texture;

Texture* texture_create(void);

void texture_load_from_file(Texture*, const char *path, SDL_Renderer*);

void texture_load_font(Texture*, const char *path, unsigned int size);

void texture_load_from_text(Texture*,
							const char *text,
							SDL_Color,
							SDL_Renderer*);

void texture_render(Texture*, Position*, Camera*);

void texture_render_clip(Texture*, Position*, SDL_Rect*, Camera*);

void texture_destroy(Texture *texture);

#endif // TEXTURE_H_
