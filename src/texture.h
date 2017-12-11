#ifndef TEXTURE_H_
#define	TEXTURE_H_

#include <SDL2/SDL.h>
#include "dimension.h"
#include "position.h"
#include "camera.h"

typedef struct {
	SDL_Texture *texture;
	Dimension dim;
	SDL_Rect clip;
} Texture;

Texture* texture_create(const char *path, SDL_Renderer *renderer);

void texture_render(Texture*, Position*, Camera*);

void texture_destroy(Texture *texture);

#endif // TEXTURE_H_
