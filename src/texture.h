#ifndef TEXTURE_H_
#define	TEXTURE_H_

#include <SDL2/SDL.h>
#include "dimension.h"

typedef struct {
	SDL_Texture *texture;
	Dimension dim;
	SDL_Rect clip;
} Texture;

Texture* texture_create(char *path, SDL_Renderer *renderer);

void texture_destroy(Texture *texture);

#endif // TEXTURE_H_
