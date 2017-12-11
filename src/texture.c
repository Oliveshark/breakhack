#include <SDL2/SDL_image.h>
#include "texture.h"
#include "util.h"

Texture* texture_create(const char *path, SDL_Renderer *renderer)
{
	Texture *newTexture = NULL;
	SDL_Surface *surface = IMG_Load(path);

	if (surface == NULL)
	{
		printf("[!!] Failed to load texture (%s): %s\n", path, IMG_GetError());
	}
	else
	{
		newTexture = ec_malloc(sizeof(Texture));
		newTexture->dim.height = surface->h;
		newTexture->dim.width = surface->w;
		newTexture->clip.x = 0;
		newTexture->clip.y = 0;
		newTexture->clip.w = surface->w;
		newTexture->clip.h = surface->h;
		newTexture->texture = SDL_CreateTextureFromSurface(renderer,
								   surface);
		if (newTexture->texture == NULL)
		{
			printf("[!!] Failed to create texture (%s): %s\n",
			       path,
			       SDL_GetError());
		}

		SDL_FreeSurface(surface);
	}

	return newTexture;
}

void 
texture_render(Texture *texture, Position *p, Camera *cam)
{
	SDL_Rect draw_box = (SDL_Rect) {
		p->x,
		p->y,
		texture->dim.width,
		texture->dim.height
	};

	SDL_RenderCopy(cam->renderer,
		       texture->texture,
		       &texture->clip,
		       &draw_box);
}

void texture_destroy(Texture *texture)
{
	SDL_DestroyTexture(texture->texture);
	free(texture);
}
