#include "actiontext.h"
#include "util.h"

ActionText*
actiontext_create()
{
	ActionText *t = ec_malloc(sizeof(ActionText));
	t->pos = (Position) { 0, 0 };
	t->texture = texture_create();
	t->timer = timer_create();
	t->active = false;
	t->color = (SDL_Color) { 255, 255, 255, 255 };
	return t;
}

void
actiontext_load_font(ActionText *t, const char *path, unsigned int size)
{
	texture_load_font(t->texture, path, size);
}

void
actiontext_set_text(ActionText *t, const char *text, SDL_Renderer *renderer)
{
	texture_load_from_text(t->texture, text, t->color, renderer);
}

void
actiontext_render(ActionText *t, Camera *cam)
{
	if (!t->active)
		return;

	if (t->active && !timer_started(t->timer))
		timer_start(t->timer);

	Position cameraPos = camera_to_camera_position(cam, &t->pos);
	if (timer_get_ticks(t->timer) < 300) {
		texture_render(t->texture, &cameraPos, cam);
	} else {
		timer_stop(t->timer);
		t->active = false;
	}
}

void
actiontext_destroy(ActionText *t)
{
	texture_destroy(t->texture);
	timer_destroy(t->timer);
	free(t);
}
