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

#include <SDL.h>
#include <SDL_ttf.h>
#include "dimension.h"
#include "position.h"
#include "camera.h"

typedef struct Texture {
	SDL_Texture *texture;
	TTF_Font *font;
	TTF_Font *outlineFont;
	Dimension dim;
	const char *path;
	unsigned long lastAccess;
	SDL_TextureAccess textureAccessType;
	bool locked;
} Texture;

Texture*
texture_create(void);

void
texture_create_blank(Texture *t,
					 SDL_TextureAccess,
					 SDL_Renderer*);

void
texture_lock(Texture*, SDL_Rect*, void **pixels, int *pitch);

void
texture_unlock(Texture*);

void
texture_load_from_file(Texture*, const char *path, SDL_Renderer*);

void
texture_load_font(Texture*, const char *path, unsigned int size, int outline);

void
texture_load_from_text(Texture*,
		       const char *text,
		       SDL_Color,
		       SDL_Color,
		       SDL_Renderer*);

void
texture_load_from_text_shaded(Texture*,
					   const char *text,
					   SDL_Color,
					   SDL_Color,
					   SDL_Renderer*);

void
texture_load_from_text_blended(Texture*,
					   const char *text,
					   SDL_Color,
					   SDL_Renderer*);

void
texture_set_blend_mode(Texture*, SDL_BlendMode);

void
texture_set_scale_mode(Texture*, SDL_ScaleMode);

void
texture_set_alpha(Texture*, Uint8);

void
texture_render(Texture*, SDL_Rect*, Camera*);

void
texture_render_clip(Texture*, SDL_Rect*, SDL_Rect*, Camera*);

void
texture_render_clip_ex(Texture*, SDL_Rect*, SDL_Rect*, double angle, SDL_Point*, SDL_RendererFlip, Camera*);

void
texture_destroy(Texture *texture);

#endif // TEXTURE_H_
