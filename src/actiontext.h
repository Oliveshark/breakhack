#ifndef ACTIONTEXT_H_
#define	ACTIONTEXT_H_

#include <SDL2/SDL.h>

#include "position.h"
#include "texture.h"
#include "timer.h"

typedef struct {
	Position pos;
	Texture *texture;
	bool active;
	Timer *timer;
	SDL_Color color;
} ActionText;

ActionText* actiontext_create();

void actiontext_load_font(ActionText*, const char *path, unsigned int size);

void actiontext_set_text(ActionText*, const char *text, SDL_Renderer*);

void actiontext_render(ActionText*, Camera*);

void actiontext_destroy(ActionText*);

#endif // ACTIONTEXT_H_
