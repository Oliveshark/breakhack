#include <stdlib.h>
#include "sprite.h"
#include "util.h"

static
Sprite* sprite_create_default(void)
{
	Sprite *s = ec_malloc(sizeof(Sprite));
	s->textures[0] = NULL;
	s->textures[1] = NULL;
	s->clip = (SDL_Rect) { 0, 0, 16, 16 };
	s->destroyTextures = false;
	s->pos = (Position) { 0, 0 };
	s->renderTimer = timer_create();
	s->texture_index = 0;
	s->fixed = false;
	s->hidden = false;

	return s;
}

Sprite* sprite_create()
{
	return sprite_create_default();
}

void 
sprite_load_texture(Sprite *sprite,
		    char *path,
		    int index,
		    SDL_Renderer *renderer)
{
	if (index > 1)
		fatal("in sprite_load_texture() index out of bounds");

	if (sprite->destroyTextures && sprite->textures[index] != NULL) {
		texture_destroy(sprite->textures[index]);
		sprite->textures[index] = NULL;
	}

	sprite->textures[index] = texture_create();
	texture_load_from_file(sprite->textures[index], path, renderer);
	sprite->destroyTextures = true;
}

void
sprite_set_texture(Sprite *s, Texture *t, int index)
{
	if (index > 1)
		fatal("in sprite_set_texture() index out of bounds");
	if (s->destroyTextures)
		fatal("in sprite_set_texture() sprite contains loaded textures");

	s->textures[index] = t;
}

void sprite_render(Sprite *s, Camera *cam)
{
	if (s->hidden)
		return;

	if (s->textures[1]) {
		if (!timer_started(s->renderTimer))
			timer_start(s->renderTimer);

		if (timer_get_ticks(s->renderTimer) > 300) {
			timer_stop(s->renderTimer);
			timer_start(s->renderTimer);
			s->texture_index = s->texture_index == 0 ? 1 : 0;
		}
	}

	Position cameraPos;
	if (!s->fixed)
		cameraPos = camera_to_camera_position(cam, &s->pos);
	else
		cameraPos = s->pos;

	texture_render_clip(s->textures[s->texture_index],
			    &cameraPos,
			    &s->clip,
			    cam);
}

void sprite_destroy(Sprite *sprite)
{
	if (sprite->destroyTextures) {
		if (sprite->textures[0])
			texture_destroy(sprite->textures[0]);
		if (sprite->textures[1])
			texture_destroy(sprite->textures[1]);
	}
	timer_destroy(sprite->renderTimer);
	free(sprite);
}
