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

Texture* texture_create();

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
