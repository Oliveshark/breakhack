#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "gui.h"
#include "util.h"
#include "map.h"

#define DEFAULT_LOG { NULL, 50, 0, 200 }

#define POS_Y_COLLECTABLES 64
#define POS_Y_XPBAR	112

static SDL_Rect frame_top_left		= { 16, 160, 16, 16 };
static SDL_Rect frame_top_right		= { 48, 160, 16, 16 };
static SDL_Rect frame_bottom_left	= { 16, 192, 16, 16 };
static SDL_Rect frame_bottom_right	= { 48, 192, 16, 16 };
static SDL_Rect frame_top		= { 32, 160, 16, 16 };
static SDL_Rect frame_bottom		= { 32, 192, 16, 16 };
static SDL_Rect frame_center		= { 32, 176, 16, 16 };
static SDL_Rect frame_left		= { 16, 176, 16, 16 };
static SDL_Rect frame_right		= { 48, 176, 16, 16 };

static struct LogData_t {
	char **log;
	unsigned int len;
	unsigned int count;
	unsigned int strlen;
} log_data = DEFAULT_LOG;

static Texture*
add_texture(Gui *gui, const char *path, SDL_Renderer *renderer)
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
gui_malloc_log(void)
{
	if (log_data.log != NULL)
		return;

	unsigned int i;

	log_data.log = ec_malloc(log_data.len * sizeof(char*));
	for (i = 0; i < log_data.len; ++i)
		log_data.log[i] = NULL;
}

static Sprite*
create_xp_sprite(Texture *t, SDL_Rect clip, Position pos)
{
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->fixed = true;
	s->clip = clip;
	s->pos = pos;
	return s;
}

static Sprite*
create_label_sprite(Position pos)
{
	Sprite *s = sprite_create();
	s->fixed = true;
	s->pos = pos;
	sprite_load_text_texture(s, "assets/GUI/SDS_8x8.ttf", 0, LABEL_FONT_SIZE);
	return s;
}

static void
init_sprites(Gui *gui, SDL_Renderer *renderer)
{
	Texture *t;
	unsigned int i;

	t = add_texture(gui, "assets/GUI/GUI0.png", renderer);

	/*
	 * Add XP bar decoration
	 */

	// Left end
	linkedlist_append(&gui->sprites, create_xp_sprite(
		t,
		(SDL_Rect) { 6 * 16, 0, 16, 16 },
		(Position) { 16, POS_Y_XPBAR }
	));

	// Right end
	linkedlist_append(&gui->sprites, create_xp_sprite(
		t,
		(SDL_Rect) { 8 * 16, 0, 16, 16 },
		(Position) { 16 + (16 * 7), POS_Y_XPBAR }
	));

	for (i = 1; i < 7; ++i) {
		linkedlist_append(&gui->sprites, create_xp_sprite(
			t,
			(SDL_Rect) { 7 * 16, 0, 16, 16 },
			(Position) { 16 + (i * 16), POS_Y_XPBAR }
		));
	}

	for (i = 0; i < 8; ++i) {
		linkedlist_append(&gui->xp_bar, create_xp_sprite(
			t,
			(SDL_Rect) { 6 * 16, 4 * 16, 16, 16 },
			(Position) { 16 + (i * 16), POS_Y_XPBAR }
		));
	}

	Sprite *s;
	t = add_texture(gui, "assets/Items/Potion.png", renderer);
	s = sprite_create();
	s->fixed = true;
	sprite_set_texture(s, t, 0);
	s->clip = (SDL_Rect) { 0, 0, 16, 16 };
	s->pos = (Position) { 16, POS_Y_COLLECTABLES };
	linkedlist_append(&gui->sprites, s);

	t = add_texture(gui, "assets/Items/Money.png", renderer);
	s = sprite_create();
	s->fixed = true;
	sprite_set_texture(s, t, 0);
	s->clip = (SDL_Rect) { 16, 16, 16, 16 };
	s->pos = (Position) { 16, POS_Y_COLLECTABLES + 16 };
	linkedlist_append(&gui->sprites, s);
}

Gui*
gui_create(SDL_Renderer *renderer)
{
	Texture *t;
	unsigned int i;

	Gui *gui = ec_malloc(sizeof(Gui));
	gui->sprites = linkedlist_create();
	gui->health = linkedlist_create();
	gui->xp_bar = linkedlist_create();
	gui->textures = ht_create(5);

	for (i = 0; i < LOG_LINES_COUNT; ++i) {
		t = texture_create();
		texture_load_font(t, "assets/GUI/SDS_8x8.ttf", LOG_FONT_SIZE);
		gui->log_lines[i] = t;
	}

	gui->labels[CURRENT_XP_LABEL] = create_label_sprite((Position) { 16, POS_Y_XPBAR + 18 });
	gui->labels[LEVEL_LABEL] = create_label_sprite((Position) { 16, POS_Y_XPBAR + 18 + 14  });
	gui->labels[DUNGEON_LEVEL_LABEL] = create_label_sprite((Position) { 16, POS_Y_XPBAR + 18 + (2*14)  });
	gui->labels[HEALTH_POTION_LABEL] = create_label_sprite((Position) { 32, POS_Y_COLLECTABLES + 5  });
	gui->labels[GOLD_LABEL] = create_label_sprite((Position) { 32, POS_Y_COLLECTABLES + 16 + 5 });

	gui_malloc_log();

	init_sprites(gui, renderer);

	return gui;
}

static void
set_max_health(Gui *gui, int max, SDL_Renderer *renderer)
{
	Texture *texture;
	int i;

	assert(max % 3 == 0);

	if (((unsigned int) max / 3) == (unsigned int) linkedlist_size(gui->health))
		return;

	// Clear sprite list
	while (gui->health != NULL)
		sprite_destroy(linkedlist_pop(&gui->health));

	texture = add_texture(gui, "assets/GUI/GUI0.png", renderer);

	for (i = 0; i < max/3; ++i) {
		Sprite *sprite = sprite_create();
		sprite->fixed = true;
		sprite->clip = (SDL_Rect) { 0, 16, 16, 16 };
		sprite->pos = (Position) { 16 + (i%8)*16, 16 + ((i-(i%8))/8)*16 };
		sprite_set_texture(sprite, texture, 0);
		linkedlist_append(&gui->health, sprite);
	}
}

static void
set_current_health(Gui *gui, int current)
{
	int partial = current % 3;
	int full = (current - partial)/3;
	int count = 0;

	if (current < 0)
		current = 0;

	LinkedList *item = gui->health;
	while (item != NULL) {
		Sprite *sprite = (Sprite*) item->data;
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

void
gui_update_player_stats(Gui *gui, Player *player, Map *map, SDL_Renderer *renderer)
{
	// TODO(Linus): Perhaps split this up a bit?
	// some static functions maybe?

	static unsigned int last_level = 0;
	static int last_xp = -1;
	static double last_gold = -1;
	static unsigned int dungeon_level = 0;
	static int max_health = -1;
	static int current_health = -1;
	static int current_potion_sips = -1;

	static SDL_Color color = { 255, 255, 255, 255 };

	unsigned int xp_from_levelup, xp_required_from_last_level;
	float xp_step, xp_current_step;
	unsigned int full_xp_blocks, partial_xp_block;
	LinkedList *xp_bars;
	unsigned int i;
	char buffer[200];

	ExperienceData data = player_get_xp_data(player);

	if (max_health != player->stats.maxhp) {
		max_health = player->stats.maxhp;
		set_max_health(gui, max_health, renderer);
	}
	if (current_health != player->stats.hp) {
		current_health = player->stats.hp;
		set_current_health(gui, current_health);
	}

	if (last_xp != (int) data.current) {
		xp_from_levelup = data.current - data.previousLevel;
		xp_required_from_last_level = data.nextLevel - data.previousLevel;
		xp_step = ((float)xp_required_from_last_level) / 32; // 4 * 8
		xp_current_step = xp_from_levelup / xp_step;

		partial_xp_block = ((unsigned int)xp_current_step) % 4;
		full_xp_blocks = (unsigned int)((xp_current_step - partial_xp_block) / 4);

		xp_bars = gui->xp_bar;
		i = 0;
		while (xp_bars != NULL) {
			Sprite *s = xp_bars->data;
			s->hidden = false;
			xp_bars = xp_bars->next;

			if (i < full_xp_blocks) {
				s->clip.x = 6 * 16;
			}
			else if (i == full_xp_blocks && partial_xp_block != 0) {
				s->clip.x = (6 * 16) + (16 * (4 - partial_xp_block));
			}
			else {
				s->hidden = true;
			}

			++i;
		}
	}

	if (dungeon_level != (unsigned int) map->level) {
		m_sprintf(buffer, 200, "Dungeon level: %d", map->level);
		texture_load_from_text(gui->labels[DUNGEON_LEVEL_LABEL]->textures[0], buffer, color, renderer);
		dungeon_level = (unsigned int) map->level;
	}

	if (current_potion_sips != (int) player->potion_sips) {
		m_sprintf(buffer, 200, "x %u", (unsigned int) player->potion_sips);
		texture_load_from_text(gui->labels[HEALTH_POTION_LABEL]->textures[0], buffer, color, renderer);
		current_potion_sips = player->potion_sips;
	}

	if (last_gold != player->gold) {
		m_sprintf(buffer, 200, "x %.2f", player->gold);
		texture_load_from_text(gui->labels[GOLD_LABEL]->textures[0], buffer, color, renderer);
		last_gold = player->gold;
	}

	if (last_xp != (int) data.current) {
		m_sprintf(buffer, 200, "XP: %u / %u", data.current, data.nextLevel);
		texture_load_from_text(gui->labels[CURRENT_XP_LABEL]->textures[0], buffer, color, renderer);
		last_xp = data.current;
	}

	if (last_level != data.level) {
		m_sprintf(buffer, 200, "Level: %u", data.level);
		texture_load_from_text(gui->labels[LEVEL_LABEL]->textures[0], buffer, color, renderer);
		last_level = data.level;
	}
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
	item = gui->xp_bar;
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

	for (int i = 0; i < LABEL_COUNT; ++i)
		sprite_render(gui->labels[i], cam);
}

void
gui_log(const char *fmt, ...)
{
	char buffer[200];
	char *new_message;
	unsigned int i;
	char tstamp[10];

	va_list args;
	va_start(args, fmt);
#ifndef _MSC_VER
	vsprintf(buffer, fmt, args);
#else // _MSC_VER
	vsprintf_s(buffer, log_data.strlen, fmt, args);
#endif // _MSC_VER
	va_end(args);

	new_message = ec_malloc(log_data.strlen * sizeof(char));

	timestamp(tstamp, 10);
	m_sprintf(new_message, log_data.strlen, "%s > %s", tstamp, buffer);

	log_data.count++;
	if (log_data.count > log_data.len) {
		log_data.count = log_data.len;
		free(log_data.log[log_data.count-1]);
		log_data.log[log_data.count-1] = NULL;
	}
	for (i = log_data.count - 1; i > 0; --i) {
		log_data.log[i] = log_data.log[i-1];
	}
	log_data.log[0] = new_message;
}

void
gui_render_log(Gui *gui, unsigned int width, unsigned int height, Camera *cam)
{
	static SDL_Color color = { 255, 255, 255, 255 };

	unsigned int i;
	unsigned int render_count;
	Position p;
	
	render_count = LOG_LINES_COUNT > log_data.count ? log_data.count : LOG_LINES_COUNT;
	p = (Position) { 16, 0 };

	gui_render_frame(gui, width/16, height/16, cam);

	for (i = 0; i < render_count; ++i) {
		Texture *t;
		p.y = 16 + ((LOG_FONT_SIZE+5) * i);
		t = gui->log_lines[i];
		texture_load_from_text(t, log_data.log[i], color, cam->renderer);
		texture_render(t, &p, cam);
	}
}

static void
destroy_log(void)
{
	if (log_data.log == NULL)
		return;

	unsigned int i;
	for (i = 0; i < log_data.count; ++i)
		free(log_data.log[i]);

	free(log_data.log);
	log_data.log = NULL;
}

void
gui_destroy(Gui *gui)
{
	destroy_log();

	while (gui->sprites != NULL)
		sprite_destroy(linkedlist_pop(&gui->sprites));
	while (gui->health != NULL)
		sprite_destroy(linkedlist_pop(&gui->health));
	while (gui->xp_bar != NULL)
		sprite_destroy(linkedlist_pop(&gui->xp_bar));

	for (int i = 0; i < LOG_LINES_COUNT; ++i)
		texture_destroy(gui->log_lines[i]);

	for (int i = 0; i < LABEL_COUNT; ++i)
		sprite_destroy(gui->labels[i]);

	ht_destroy_custom(gui->textures, (void (*)(void*)) &texture_destroy);
	free(gui);
}
