#include <assert.h>
#include <stdlib.h>

#include "gui.h"
#include "util.h"

static SDL_Rect frame_top_left	= { 16, 160, 16, 16 };
static SDL_Rect frame_top_right	= { 48, 160, 16, 16 };
static SDL_Rect frame_bottom_left	= { 16, 192, 16, 16 };
static SDL_Rect frame_bottom_right	= { 48, 192, 16, 16 };
static SDL_Rect frame_top		= { 32, 160, 16, 16 };
static SDL_Rect frame_bottom		= { 32, 192, 16, 16 };
static SDL_Rect frame_center		= { 32, 176, 16, 16 };
static SDL_Rect frame_left		= { 16, 176, 16, 16 };
static SDL_Rect frame_right		= { 48, 176, 16, 16 };

static char **gui_log;
static unsigned int log_length = 50;

static void
gui_malloc_log(void)
{
	static bool log_allocated = false;
	if (log_allocated)
		return;

	unsigned int i;

	gui_log = ec_malloc(log_length * sizeof(char*));
	for (i = 0; i < log_length; ++i)
		gui_log[i] = ec_malloc(200 * sizeof(char));

	log_allocated = true;
}

Gui*
gui_create()
{
	Gui *gui = ec_malloc(sizeof(Gui));
	gui->sprites = linkedlist_create();
	gui->health = linkedlist_create();
	gui->textures = ht_create(5);

	gui_malloc_log();

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
	Texture *texture;
	int i;

	assert(max % 3 == 0);

	if (((unsigned int) max / 3) == (unsigned int) linkedlist_size(gui->health))
		return;

	clear_sprite_list(gui->health);

	texture = gui_add_texture(gui, "assets/GUI/GUI0.png", renderer);

	for (i = 0; i < max/3; ++i) {
		Sprite *sprite = sprite_create();
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

static void
gui_render_frame(Gui *gui, unsigned int width, unsigned int height, Camera *cam)
{
	Texture *texture = ht_get(gui->textures, "assets/GUI/GUI0.png");
	Position pos = { 0, 0 };
	unsigned int i, j;

	for (i = 0; i < width; ++i) {
		for (j = 0; j < height; ++j) {
			pos.x = i * 16;
			pos.y = j * 16;

			if (i == 0 && j == 0) {
				texture_render_clip(texture, &pos, &frame_top_left, cam);
			} else if (i == (width - 1) && j == 0) {
				texture_render_clip(texture, &pos, &frame_top_right, cam);
			} else if (i == 0 && j == (height - 1)) {
				texture_render_clip(texture, &pos, &frame_bottom_left, cam);
			} else if (i == (width - 1) && j == (height - 1)) {
				texture_render_clip(texture, &pos, &frame_bottom_right, cam);
			} else if (i == 0) {
				texture_render_clip(texture, &pos, &frame_left, cam);
			} else if (i == (width - 1)) {
				texture_render_clip(texture, &pos, &frame_right, cam);
			} else if (j == 0) {
				texture_render_clip(texture, &pos, &frame_top, cam);
			} else if (j == (height - 1)) {
				texture_render_clip(texture, &pos, &frame_bottom, cam);
			} else {
				texture_render_clip(texture, &pos, &frame_center, cam);
			}
		}
	}
}

void
gui_render_panel(Gui *gui, unsigned int width, unsigned int height, Camera *cam)
{
	gui_render_frame(gui, width/16, height/16, cam);

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
gui_render_log(Gui *gui, unsigned int width, unsigned int height, Camera *cam)
{
	gui_render_frame(gui, width/16, height/16, cam);
}

static void
destroy_log(void)
{
	if (gui_log == NULL)
		return;

	unsigned int i;
	for (i = 0; i < log_length; ++i)
		free(gui_log[i]);

	free(gui_log);
	gui_log = NULL;
}

void
gui_destroy(Gui *gui)
{
	destroy_log();

	while (gui->sprites != NULL)
		sprite_destroy(linkedlist_pop(&gui->sprites));
	while (gui->health != NULL)
		sprite_destroy(linkedlist_pop(&gui->health));

	ht_destroy_custom(gui->textures, (void (*)(void*)) &texture_destroy);
	free(gui);
}