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

#include <SDL_image.h>
#include <stdio.h>
#include <physfs.h>
#include <stdlib.h>
#include "texture.h"
#include "util.h"
#include "io_util.h"

Texture*
texture_create(void)
{
	Texture *t = ec_malloc(sizeof(Texture));
	t->dim.height = 0;
	t->dim.width = 0;
	t->texture = NULL;
	t->font = NULL;
	t->lastAccess = SDL_GetTicks();
	t->path = "";
	return t;
}

void
texture_load_from_file(Texture *texture,
		       const char *path,
		       SDL_Renderer *renderer)
{
	SDL_Surface *surface = IMG_Load_RW(io_load_rwops(path), true);

	if (surface == NULL)
	{
		error("Failed to load texture (%s): %s",
		       path, IMG_GetError());
		return;
	}

	if (texture->texture) {
		SDL_DestroyTexture(texture->texture);
		texture->texture = NULL;
	}

	texture->dim.height = surface->h;
	texture->dim.width = surface->w;
	texture->texture = SDL_CreateTextureFromSurface(renderer,
							surface);
	if (texture->texture == NULL)
	{
		error("Failed to create texture (%s): %s",
		       path,
		       SDL_GetError());
	}

	texture->lastAccess = SDL_GetTicks();
	texture->path = path;

	SDL_FreeSurface(surface);
}

void
texture_load_font(Texture *t, const char *path, unsigned int size)
{
	if (t->font)
		TTF_CloseFont(t->font);

	t->font = TTF_OpenFontRW(io_load_rwops(path), true, size);

	if (t->font == NULL) {
		error("Failed to load font %s: %s",
			path,
			TTF_GetError());
		return;
	}
	t->path = path;
}

static void
load_from_surface(Texture *t, SDL_Surface *surface, SDL_Renderer *renderer)
{
	if (t->texture) {
		SDL_DestroyTexture(t->texture);
		t->texture = NULL;
	}

	t->texture = SDL_CreateTextureFromSurface( renderer, surface );
	if (t->texture == NULL) {
		error("Failed to create texture from text: %s",
		       SDL_GetError());
		return;
	}

	t->dim.width = surface->w;
	t->dim.height = surface->h;

	SDL_FreeSurface(surface);
}

void
texture_load_from_text(Texture *t,
		       const char *text,
		       SDL_Color c,
		       SDL_Renderer *renderer)
{
	SDL_Surface *surface = TTF_RenderText_Solid( t->font, text, c );
	if (surface == NULL)
	{
		error("Unable to create texture from rendered text: %s",
		       IMG_GetError());
		return;
	}

	load_from_surface(t, surface, renderer);
	t->lastAccess = SDL_GetTicks();
}

void
texture_load_from_text_shaded(Texture *t, const char * text, SDL_Color fg, SDL_Color bg, SDL_Renderer *renderer)
{
	SDL_Surface *surface = TTF_RenderText_Shaded( t->font, text, fg, bg );
	if (surface == NULL)
	{
		error("Unable to create texture from rendered text: %s",
		       IMG_GetError());
		return;
	}

	load_from_surface(t, surface, renderer);
}

void
texture_load_from_text_blended(Texture *t, const char * text, SDL_Color fg, SDL_Renderer *renderer)
{
	SDL_Surface *surface = TTF_RenderText_Blended( t->font, text, fg );
	if (surface == NULL)
	{
		error("Unable to create texture from rendered text: %s",
		       IMG_GetError());
		return;
	}

	load_from_surface(t, surface, renderer);
}

void 
texture_render(Texture *texture, SDL_Rect *box, Camera *cam)
{
	if (!texture->texture)
		return;

	texture_render_clip(texture, box, NULL, cam);
}

void 
texture_render_clip(Texture *texture, SDL_Rect *box, SDL_Rect *clip, Camera *cam)
{
	if (!texture->texture)
		return;

	SDL_RenderCopy(cam->renderer,
		       texture->texture,
		       clip,
		       box);

	texture->lastAccess = SDL_GetTicks();
}

void 
texture_render_clip_ex(Texture *texture, SDL_Rect *box, SDL_Rect *clip, double angle, SDL_Point *point, SDL_RendererFlip flipType, Camera *cam)
{
	if (!texture->texture)
		return;

	SDL_RenderCopyEx(cam->renderer,
			 texture->texture,
			 clip,
			 box,
			 angle,
			 point,
			 flipType);

	texture->lastAccess = SDL_GetTicks();
}

void texture_destroy(Texture *texture)
{
	if (texture->texture)
		SDL_DestroyTexture(texture->texture);
	if (texture->font)
		TTF_CloseFont(texture->font);
	free(texture);
}
