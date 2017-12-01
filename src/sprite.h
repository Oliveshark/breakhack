#ifndef SPRITE_H_
#define	SPRITE_H_

#include <stdbool.h>

#include "texture.h"
#include "position.h"
#include "camera.h"

typedef struct Sprite_t{
	Texture *texture;
	Position pos;
	void (*handle_event)(struct Sprite_t *sprite, SDL_Event*);
} Sprite;

Sprite* sprite_create();

void sprite_load_texture(Sprite *, char *path, SDL_Renderer *);

void sprite_render(Sprite*, Camera*);

void sprite_destroy(Sprite *);

#endif // SPRITE_H_
