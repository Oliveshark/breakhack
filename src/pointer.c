#include <stdbool.h>
#include <stdlib.h>
#include "pointer.h"
#include "util.h"
#include "particle_engine.h"

Pointer *
pointer_create(SDL_Renderer *renderer)
{
	SDL_ShowCursor(SDL_DISABLE);

	Pointer *p = ec_malloc(sizeof(Pointer));
	p->sprite = sprite_create();
	sprite_load_texture(p->sprite, "assets/Items/MedWep.png", 0, renderer);
	p->sprite->fixed = true;
	p->sprite->clip = (SDL_Rect) { 0, 0, 16, 16 };
	p->sprite->textures[0]->dim = (Dimension) {
		TILE_DIMENSION, TILE_DIMENSION };

	return p;
}

void
pointer_handle_event(Pointer *p, SDL_Event *event)
{
	if (event->type == SDL_MOUSEMOTION) {
		p->sprite->pos.x = event->motion.x;
		p->sprite->pos.y = event->motion.y;
		// debug("Pointer pos: %dx%d", p->sprite->pos.x, p->sprite->pos.y);
	}
}

void
pointer_toggle_clickable_pointer(Pointer *p, bool clickable)
{
	if (clickable)
		p->sprite->clip.x = 32;
	else
		p->sprite->clip.x = 0;
}

void
pointer_render(Pointer *p, Camera *cam)
{
	sprite_render(p->sprite, cam);
}

void
pointer_destroy(Pointer *p)
{
	SDL_ShowCursor(SDL_ENABLE);

	sprite_destroy(p->sprite);
	free(p);
}
