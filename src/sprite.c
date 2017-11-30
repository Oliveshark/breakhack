#include "sprite.h"
#include "util.h"

static
Sprite* sprite_create_default()
{
	Position pos = { 0, 0 };

	Sprite *s = ec_malloc(sizeof(Sprite));
	*s = (Sprite) { NULL, pos, NULL };

	return s;
}

Sprite* sprite_create()
{
	return sprite_create_default();
}

void sprite_load_texture(Sprite *sprite, char *path, SDL_Renderer *renderer)
{
	if (sprite->texture != NULL) {
		texture_destroy(sprite->texture);
		sprite->texture = NULL;
	}

	sprite->texture = texture_create(path, renderer);
}

void sprite_render(Sprite *s, SDL_Renderer *renderer)
{
	SDL_Rect draw_box = (SDL_Rect) {
		s->pos.x,
		s->pos.y,
		s->texture->dim.width,
		s->texture->dim.height
	};

	SDL_RenderCopy(renderer,
		       s->texture->texture,
		       &s->texture->clip,
		       &draw_box);
}

void sprite_destroy(Sprite *sprite)
{
	if (sprite->texture)
		texture_destroy(sprite->texture);
	free(sprite);
}
