#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "texture.h"
#include "util.h"

Texture*
texture_create()
{
	Texture *t = ec_malloc(sizeof(Texture));
	t->dim.height = 0;
	t->dim.width = 0;
	t->texture = NULL;
	t->font = NULL;
	return t;
}

void
texture_load_from_file(Texture *texture,
		       const char *path,
		       SDL_Renderer *renderer)
{
	SDL_Surface *surface = IMG_Load(path);

	if (surface == NULL)
	{
		printf("[!!] Failed to load texture (%s): %s\n",
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
		printf("[!!] Failed to create texture (%s): %s\n",
		       path,
		       SDL_GetError());
	}

	SDL_FreeSurface(surface);
}

void
texture_load_font(Texture *t, const char *path, unsigned int size)
{
	if (t->font)
		TTF_CloseFont(t->font);
	t->font = TTF_OpenFont(path, size);
	if (t->font == NULL) {
		fprintf(stderr, "[!!] Failed to load font %s: %s\n",
			path,
			TTF_GetError());
		return;
	}
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
		printf("[!!] Unable to create texture from rendered text: %s\n",
		       IMG_GetError());
		return;
	}

	if (t->texture) {
		SDL_DestroyTexture(t->texture);
		t->texture = NULL;
	}

	t->texture = SDL_CreateTextureFromSurface( renderer, surface );
	if (t->texture == NULL) {
		printf("[!!] Failed to create texture from text: %s\n",
		       SDL_GetError());
		return;
	}

	t->dim.width = surface->w;
	t->dim.height = surface->h;

	SDL_FreeSurface(surface);
}

void 
texture_render(Texture *texture, Position *p, Camera *cam)
{
	if (!texture->texture)
		return;

	SDL_Rect draw_box = (SDL_Rect) {
		p->x,
		p->y,
		texture->dim.width,
		texture->dim.height
	};

	SDL_Rect clip = (SDL_Rect) {
		0,
		0,
		texture->dim.width,
		texture->dim.height
	};

	SDL_RenderCopy(cam->renderer,
		       texture->texture,
		       &clip,
		       &draw_box);
}

void 
texture_render_clip(Texture *texture, Position *p, SDL_Rect *clip, Camera *cam)
{
	SDL_Rect draw_box = (SDL_Rect) {
		p->x,
		p->y,
		texture->dim.width,
		texture->dim.height
	};

	SDL_RenderCopy(cam->renderer,
		       texture->texture,
		       clip,
		       &draw_box);
}

void texture_destroy(Texture *texture)
{
	if (texture->texture)
		SDL_DestroyTexture(texture->texture);
	if (texture->font)
		TTF_CloseFont(texture->font);
	free(texture);
}
