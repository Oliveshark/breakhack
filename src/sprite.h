#ifndef SPRITE_H_
#define	SPRITE_H_

#include <stdbool.h>

#include "texture.h"
#include "position.h"
#include "camera.h"
#include "roommatrix.h"
#include "timer.h"

typedef struct Sprite_t {
	Texture* textures[2];
	SDL_Rect clip;
	bool destroyTextures;
	Position pos;
	Timer *renderTimer;
	unsigned int texture_index;
	bool fixed;
} Sprite;

Sprite* sprite_create(void);

void sprite_load_texture(Sprite *, char *path, int index, SDL_Renderer *);

void sprite_set_texture(Sprite *, Texture *, int index);

void sprite_render(Sprite*, Camera*);

void sprite_destroy(Sprite *);

#endif // SPRITE_H_
