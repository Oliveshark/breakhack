#ifndef POINTER_H_
#define	POINTER_H_

#include <SDL2/SDL.h>
#include "sprite.h"
#include "camera.h"

typedef struct Pointer_t {
	Sprite *sprite;
} Pointer;

Pointer *
pointer_create(SDL_Renderer *renderer);

void
pointer_handle_event(Pointer*, SDL_Event *event);

void
pointer_render(Pointer*, Camera*);

void
pointer_destroy(Pointer*);

#endif // POINTER_H_
