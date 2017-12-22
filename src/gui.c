#include <assert.h>

#include "gui.h"
#include "util.h"

Gui*
gui_create()
{
	Gui *gui = ec_malloc(sizeof(Gui));
	gui->sprites = linkedlist_create();
	gui->health = linkedlist_create();
	gui->textures = ht_create(5);
	return gui;
}

static void
clear_sprite_list(LinkedList *list)
{
	while (list != NULL)
		sprite_destroy(linkedlist_pop(&list));
}

void
gui_set_max_health(Gui *gui, int max, SDL_Renderer *renderer)
{
	Sprite *sprite;
	Texture *texture;
	int i;

	assert(max % 3 == 0);

	if (((unsigned int) max / 3) == (unsigned int) linkedlist_size(gui->health))
		return;

	clear_sprite_list(gui->health);

	texture = gui_add_texture(gui, "assets/GUI/GUI0.png", renderer);

	for (i = 0; i < max/3; ++i) {
		sprite = sprite_create();
		sprite->fixed = true;
		sprite->clip = (SDL_Rect) { 0, 16, 16, 16 };
		sprite->pos = (Position) { 16 + i*16, 16 };
		sprite_set_texture(sprite, texture, 0);
		linkedlist_append(&gui->health, sprite);
	}
}

void
gui_set_current_health(Gui *gui, int current)
{
	Sprite *sprite;
	int partial = current % 3;
	int full = (current - partial)/3;
	int count = 0;

	if (current < 0)
		current = 0;

	LinkedList *item = gui->health;
	while (item != NULL) {
		sprite = (Sprite*) item->data;
		if (count < full) {
			sprite->clip.x = 0;
		} else if (count == full) {
			sprite->clip.x = 64 - (partial * 16);
		} else {
			sprite->clip.x = 64;
		}

		++count;
		item = item->next;
	}
}

Texture*
gui_add_texture(Gui *gui, const char *path, SDL_Renderer *renderer)
{
	Texture *t = ht_get(gui->textures, path);
	if (t == NULL) {
		t = texture_create();
		texture_load_from_file(t, path, renderer);
		t->dim = (Dimension) { 16, 16 };
		ht_set(gui->textures, path, t);
	}
	return t;
}

void
gui_render(Gui *gui, Camera *cam)
{
	LinkedList *item = gui->health;
	while (item != NULL) {
		Sprite *s = item->data;
		sprite_render(s, cam);
		item = item->next;
	}
	item = gui->sprites;
	while (item != NULL) {
		Sprite *s = item->data;
		sprite_render(s, cam);
		item = item->next;
	}

}

void
gui_destroy(Gui *gui)
{
	while (gui->sprites != NULL)
		sprite_destroy(linkedlist_pop(&gui->sprites));
	while (gui->health != NULL)
		sprite_destroy(linkedlist_pop(&gui->health));
	ht_destroy_custom(gui->textures, (void (*)(void*)) &texture_destroy);
	free(gui);
}
