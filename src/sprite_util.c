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

#include "sprite_util.h"
#include "util.h"

static struct {
	SDL_Renderer *renderer;
} utilData;

void
sprite_util_init(SDL_Renderer *renderer)
{
	utilData.renderer = renderer;
}

Sprite *
sprite_util_create_text_sprite(const char *fontPath,
			       int size,
			       SDL_Color fg,
			       SDL_Color outline,
			       const char *fmt,
			       ...)
{
	va_list args;
	Sprite *sprite = sprite_create();

	sprite_load_text_texture(sprite, fontPath, 0, size, 1);
	char priceLabel[10];

	va_start(args, fmt);
	m_vsprintf(priceLabel, 10, fmt, args);
	va_end(args);

	texture_load_from_text(sprite->textures[0],
			       priceLabel,
			       fg,
			       outline,
			       utilData.renderer);

	sprite->dim = sprite->textures[0]->dim;

	return sprite;
}
