/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2025  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "gui.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "defines.h"
#include "map_room_modifiers.h"
#include "roommatrix.h"
#include "texture.h"
#include "util.h"
#include "map.h"
#include "texturecache.h"
#include "gui_util.h"

#define DEFAULT_EVENT_MESSAGES {NULL, 5, 0, LOG_LINES_MAX_LEN}

#define POS_Y_COLLECTABLES 64
#define POS_Y_XPBAR        128

static struct LogData_t {
	char log[LOG_LINES_COUNT][LOG_LINES_MAX_LEN];
	uint8_t capacity;
	uint8_t head;
	uint8_t tail;
	uint8_t count;
	uint8_t strlen;
} log_data = {{"", "", "", "", "", "", "", "", "", ""}, LOG_LINES_COUNT, 0, 0, 0, LOG_LINES_MAX_LEN};

static struct GuiEventMsgs {
	char **messages;
	unsigned int len;
	unsigned int count;
	unsigned int strlen;
} event_messages = DEFAULT_EVENT_MESSAGES;

static void
gui_malloc_eventmessages(void)
{
	if (event_messages.messages != NULL)
		return;

	unsigned int i;

	event_messages.messages = ec_malloc(event_messages.len * sizeof(char *));
	for (i = 0; i < event_messages.len; ++i)
		event_messages.messages[i] = NULL;
}

static Sprite *
create_xp_sprite(Texture *t, SDL_Rect clip, Position pos)
{
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->fixed = true;
	s->clip = clip;
	s->pos = pos;
	return s;
}

static Sprite *
create_label_sprite(Position pos)
{
	Sprite *s = sprite_create();
	s->fixed = true;
	s->pos = pos;
	sprite_load_text_texture(s, "GUI/SDS_8x8.ttf", 0, LABEL_FONT_SIZE, 0);
	return s;
}

static Sprite *
create_minimap_blank_sprite(SDL_Renderer *renderer)
{
	Sprite *sprite = sprite_create();
	Texture *texture = texture_create();
	texture->dim = (Dimension){RIGHT_GUI_WIDTH, MINIMAP_GUI_HEIGHT - 8};
	sprite->textures[0] = texture;
	sprite->destroyTextures = true;
	sprite->pos = (Position){0, 4};
	sprite->dim = (Dimension){RIGHT_GUI_WIDTH, MINIMAP_GUI_HEIGHT - 8};
	sprite->fixed = true;
	texture_create_blank(texture, SDL_TEXTUREACCESS_TARGET, renderer);
	texture_set_blend_mode(texture, SDL_BLENDMODE_BLEND);
	texture_set_scale_mode(texture, SDL_SCALEMODE_NEAREST);

	return sprite;
}

static void
init_sprites(Gui *gui, Camera *cam)
{
	Texture *t;
	unsigned int i;

	t = texturecache_add("GUI/GUI0.png");

	/*
	 * Add XP bar decoration
	 */

	// Left end
	linkedlist_append(&gui->sprites, create_xp_sprite(t, (SDL_Rect){6 * 16, 0, 16, 16}, POS(16, POS_Y_XPBAR)));

	// Right end
	linkedlist_append(&gui->sprites,
	                  create_xp_sprite(t, (SDL_Rect){8 * 16, 0, 16, 16}, POS(16 + (16 * 7), POS_Y_XPBAR)));

	for (i = 1; i < 7; ++i) {
		linkedlist_append(&gui->sprites,
		                  create_xp_sprite(t, (SDL_Rect){7 * 16, 0, 16, 16}, POS(16 + (i * 16), POS_Y_XPBAR)));
	}

	for (i = 0; i < 8; ++i) {
		linkedlist_append(&gui->xp_bar, create_xp_sprite(t, (SDL_Rect){6 * 16, 4 * 16, 16, 16},
		                                                 POS(16 + (i * 16), POS_Y_XPBAR)));
	}

	Sprite *s;
	t = texturecache_add("Items/Potion.png");
	s = sprite_create();
	s->fixed = true;
	sprite_set_texture(s, t, 0);
	s->clip = CLIP16(0, 0);
	s->pos = POS(16, POS_Y_COLLECTABLES);
	linkedlist_append(&gui->sprites, s);

	t = texturecache_add("Items/Money.png");
	s = sprite_create();
	s->fixed = true;
	sprite_set_texture(s, t, 0);
	s->clip = CLIP16(16, 16);
	s->pos = POS(16, POS_Y_COLLECTABLES + 16);
	linkedlist_append(&gui->sprites, s);

	t = texturecache_add("Items/ShortWep.png");
	s = sprite_create();
	s->fixed = true;
	sprite_set_texture(s, t, 0);
	s->clip = CLIP16(0, 0);
	s->pos = POS(16, POS_Y_COLLECTABLES + 32);
	linkedlist_append(&gui->sprites, s);

	t = texturecache_add("Extras/Keys.png");
	s = sprite_create();
	s->fixed = true;
	sprite_set_texture(s, t, 0);
	s->clip = CLIP16(0, 0);
	s->pos = POS(58, POS_Y_XPBAR + 15 + (3 * 14));
	gui->silverKey = s;

	t = texturecache_add("Extras/Keys.png");
	s = sprite_create();
	s->fixed = true;
	sprite_set_texture(s, t, 0);
	s->clip = CLIP16(16, 0);
	s->pos = POS(74, POS_Y_XPBAR + 15 + (3 * 14));
	gui->goldKey = s;

	gui->statsFrame = gui_util_create_frame_sprite(RIGHT_GUI_WIDTH / 16, STATS_GUI_HEIGHT / 16, cam);
	gui->bottomFrame = gui_util_create_frame_sprite(BOTTOM_GUI_WIDTH / 16, BOTTOM_GUI_HEIGHT / 16, cam);

	gui->miniMap = create_minimap_blank_sprite(cam->renderer);
	gui->miniMapOverlay = create_minimap_blank_sprite(cam->renderer);

	texture_load_from_text(gui->labels[KEY_LABEL]->textures[0], "Keys:", C_WHITE, C_BLACK, cam->renderer);
	gui->labels[KEY_LABEL]->dim = gui->labels[KEY_LABEL]->textures[0]->dim;
}

Gui *
gui_create(Camera *cam)
{
	Texture *t;
	unsigned int i;

	Gui *gui = ec_malloc(sizeof(Gui));
	gui->sprites = linkedlist_create();
	gui->health = linkedlist_create();
	gui->xp_bar = linkedlist_create();
	gui->activeTooltip = NULL;

	for (i = 0; i < LOG_LINES_COUNT; ++i) {
		t = texture_create();
		texture_load_font(t, "GUI/SDS_8x8.ttf", LOG_FONT_SIZE, 0);
		gui->log_lines[i] = t;
	}

	gui->event_message = texture_create();
	texture_load_font(gui->event_message, "GUI/SDS_8x8.ttf", EVENT_MESSAGE_FONT_SIZE, 2);
	gui->event_message_timer = _timer_create();

	gui->labels[CURRENT_XP_LABEL] = create_label_sprite(POS(16, POS_Y_XPBAR + 18));
	gui->labels[LEVEL_LABEL] = create_label_sprite(POS(16, POS_Y_XPBAR + 18 + 14));
	gui->labels[DUNGEON_LEVEL_LABEL] = create_label_sprite(POS(16, POS_Y_XPBAR + 18 + (2 * 14)));
	gui->labels[KEY_LABEL] = create_label_sprite(POS(16, POS_Y_XPBAR + 18 + (3 * 14)));
	gui->labels[HEALTH_POTION_LABEL] = create_label_sprite(POS(32, POS_Y_COLLECTABLES + 5));
	gui->labels[GOLD_LABEL] = create_label_sprite(POS(32, POS_Y_COLLECTABLES + 16 + 5));
	gui->labels[DAGGER_LABEL] = create_label_sprite(POS(32, POS_Y_COLLECTABLES + 32 + 5));

	gui_malloc_eventmessages();

	init_sprites(gui, cam);

	return gui;
}

static void
set_max_health(Gui *gui, int max)
{
	Texture *texture0, *texture1;
	int i;

	assert(max % 3 == 0);

	if (((unsigned int)max / 3) == (unsigned int)linkedlist_size(gui->health))
		return;

	// Clear sprite list
	while (gui->health != NULL)
		sprite_destroy(linkedlist_pop(&gui->health));

	texture0 = texturecache_add("GUI/GUI0.png");
	texture1 = texturecache_add("GUI/GUI1.png");

	for (i = 0; i < max / 3; ++i) {
		Sprite *sprite = sprite_create();
		sprite->fixed = true;
		sprite->animate = false;
		sprite->clip = (SDL_Rect){0, 16, 16, 16};
		sprite->pos = POS(16 + (i % 8) * 16, 16 + ((i - (i % 8)) / 8) * 16);
		sprite_set_texture(sprite, texture0, 0);
		sprite_set_texture(sprite, texture1, 1);
		linkedlist_append(&gui->health, sprite);
	}
}

static void
set_current_health(Gui *gui, int current)
{
	if (current < 0)
		current = 0;

	int partial = current % 3;
	int full = (current - partial) / 3;
	int count = 0;

	LinkedList *item = gui->health;
	while (item != NULL) {
		Sprite *sprite = (Sprite *)item->data;
		if (count < full) {
			sprite->clip.x = 0;
		} else if (count == full) {
			sprite->clip.x = 64 - (partial * 16);
		} else {
			sprite->clip.x = 64;
		}

		sprite->animate = current < 9;

		++count;
		item = item->next;
	}
}

static void
update_xp_bar(Gui *gui, ExperienceData *data)
{
	unsigned int xp_from_levelup = data->current - data->previousLevel;
	unsigned int xp_required_from_last_level = data->nextLevel - data->previousLevel;
	float xp_step = ((float)xp_required_from_last_level) / 32; // 4 * 8
	float xp_current_step = (float)xp_from_levelup / xp_step;

	unsigned int partial_xp_block = ((unsigned int)xp_current_step) % 4;
	unsigned int full_xp_blocks = (unsigned int)((xp_current_step - (float)partial_xp_block) / 4);

	LinkedList *xp_bars = gui->xp_bar;
	unsigned int i = 0;
	while (xp_bars != NULL) {
		Sprite *s = xp_bars->data;
		s->hidden = false;
		xp_bars = xp_bars->next;

		if (i < full_xp_blocks) {
			s->clip.x = 6 * 16;
		} else if (i == full_xp_blocks && partial_xp_block != 0) {
			s->clip.x = (6 * 16) + (16 * (4 - partial_xp_block));
		} else {
			s->hidden = true;
		}

		++i;
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
	static int current_dagger_count = -1;

	char buffer[200];

	ExperienceData data = player_get_xp_data(player);

	if (max_health != player->stats.maxhp) {
		max_health = player->stats.maxhp;
		set_max_health(gui, max_health);
	}
	if (current_health != player->stats.hp) {
		current_health = player->stats.hp;
		set_current_health(gui, current_health);
	}

	if (last_xp != (int)data.current) {
		update_xp_bar(gui, &data);
	}

	if (dungeon_level != (unsigned int)map->level) {
		m_sprintf(buffer, 200, "Dungeon level: %d", map->level);
		texture_load_from_text(gui->labels[DUNGEON_LEVEL_LABEL]->textures[0], buffer, C_WHITE, C_BLACK,
		                       renderer);
		gui->labels[DUNGEON_LEVEL_LABEL]->dim = gui->labels[DUNGEON_LEVEL_LABEL]->textures[0]->dim;
		dungeon_level = (unsigned int)map->level;
	}

	if (current_potion_sips != (int)player->potion_sips) {
		m_sprintf(buffer, 200, "x %u", (unsigned int)player->potion_sips);
		texture_load_from_text(gui->labels[HEALTH_POTION_LABEL]->textures[0], buffer, C_WHITE, C_BLACK,
		                       renderer);
		gui->labels[HEALTH_POTION_LABEL]->dim = gui->labels[HEALTH_POTION_LABEL]->textures[0]->dim;
		current_potion_sips = player->potion_sips;
	}

	if (current_dagger_count != (int)player->daggers) {
		m_sprintf(buffer, 200, "x %u", (unsigned int)player->daggers);
		texture_load_from_text(gui->labels[DAGGER_LABEL]->textures[0], buffer, C_WHITE, C_BLACK, renderer);
		gui->labels[DAGGER_LABEL]->dim = gui->labels[DAGGER_LABEL]->textures[0]->dim;
		current_dagger_count = (int)player->daggers;
	}

	if (last_gold != player->gold) {
		m_sprintf(buffer, 200, "x %.2f", player->gold);
		texture_load_from_text(gui->labels[GOLD_LABEL]->textures[0], buffer, C_WHITE, C_BLACK, renderer);
		gui->labels[GOLD_LABEL]->dim = gui->labels[GOLD_LABEL]->textures[0]->dim;
		last_gold = player->gold;
	}

	if (last_xp != (int)data.current) {
		m_sprintf(buffer, 200, "XP: %u / %u", data.current, data.nextLevel);
		texture_load_from_text(gui->labels[CURRENT_XP_LABEL]->textures[0], buffer, C_WHITE, C_BLACK, renderer);
		gui->labels[CURRENT_XP_LABEL]->dim = gui->labels[CURRENT_XP_LABEL]->textures[0]->dim;
		last_xp = data.current;
	}

	if (last_level != data.level) {
		m_sprintf(buffer, 200, "Level: %u", data.level);
		texture_load_from_text(gui->labels[LEVEL_LABEL]->textures[0], buffer, C_WHITE, C_BLACK, renderer);
		gui->labels[LEVEL_LABEL]->dim = gui->labels[LEVEL_LABEL]->textures[0]->dim;
		last_level = data.level;
	}

	gui->silverKey->hidden = !(player->equipment.keys & LOCK_SILVER);
	gui->goldKey->hidden = !(player->equipment.keys & LOCK_GOLD);
	gui->goldKey->pos.x = gui->silverKey->hidden ? 58 : 74;
}

void
gui_render_panel(Gui *gui, Camera *cam)
{
	sprite_render(gui->statsFrame, cam);
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
	sprite_render(gui->silverKey, cam);
	sprite_render(gui->goldKey, cam);
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
gui_update_minimap(Gui *gui, Camera *cam, RoomMatrix *rm)
{
	SDL_SetRenderTarget(cam->renderer, gui->miniMap->textures[0]->texture);

	for (size_t i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (size_t j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			const RoomSpace *space = &rm->spaces[i][j];
			const float x = (float)(i + rm->roomPos.x * MAP_ROOM_WIDTH);
			const float y = (float)(j + rm->roomPos.y * MAP_ROOM_HEIGHT);

			SDL_Color c = {0, 0, 0, SDL_ALPHA_OPAQUE};
			if (SPACE_IS_LETHAL(space)) {
				c.a = SDL_ALPHA_TRANSPARENT;
			} else if (space->trap) {
				c.r = 255;
			} else if (space->door) {
				c.r = 0;
				c.g = 0;
				c.b = 255;
			} else if (space->tile && space->tile->levelExit) {
				c.r = 0;
				c.g = 255;
				c.b = 0;
			} else if (space->wall || SPACE_IS_OCCUPIED(space)) {
				c.r = 200;
				c.g = 200;
				c.b = 200;
			} else if (space->tile == NULL) {
				c.a = SDL_ALPHA_TRANSPARENT;
			} else if (SPACE_IS_WALKABLE(space)) {
				c.r = 94;
				c.g = 77;
				c.b = 179;
			} else {
				c.r = 200;
				c.g = 200;
				c.b = 200;
			}

			SDL_SetRenderDrawColor(cam->renderer, c.r, c.g, c.b, c.a);
			SDL_RenderPoint(cam->renderer, x, y);
		}
	}

	if (rm->modifier && rm->modifier->type != RMOD_TYPE_NONE) {
		const SDL_FRect mod_box = {
		    (float)rm->roomPos.x * MAP_ROOM_WIDTH,
		    (float)rm->roomPos.y * MAP_ROOM_HEIGHT,
		    (float)MAP_ROOM_WIDTH,
		    (float)MAP_ROOM_HEIGHT,
		};
		switch (rm->modifier->type) {
			case RMOD_TYPE_WINDY:
				SDL_SetRenderDrawColor(cam->renderer, 0, 0, 255, 100);
				break;
			case RMOD_TYPE_FIRE:
				SDL_SetRenderDrawColor(cam->renderer, 255, 0, 0, 100);
				break;
			case RMOD_TYPE_CRUMBLING:
				SDL_SetRenderDrawColor(cam->renderer, 247, 151, 67, 100);
				break;
			default:
				assert(false);
		}
		SDL_RenderFillRect(cam->renderer, &mod_box);
	}

	SDL_SetRenderTarget(cam->renderer, NULL);
}

void
gui_update_minimap_pos(Gui *gui, Camera *cam, RoomMatrix *rm)
{
	SDL_SetRenderTarget(cam->renderer, gui->miniMapOverlay->textures[0]->texture);
	SDL_SetRenderDrawColor(cam->renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
	SDL_RenderClear(cam->renderer);

	// Render current room square
	const SDL_FRect r = {(float)rm->roomPos.x * MAP_ROOM_WIDTH, (float)rm->roomPos.y * MAP_ROOM_HEIGHT,
	                     MAP_ROOM_WIDTH, MAP_ROOM_HEIGHT};
	SDL_SetRenderDrawColor(cam->renderer, 235, 235, 52, 200);
	SDL_RenderRect(cam->renderer, &r);

	SDL_SetRenderTarget(cam->renderer, NULL);
}

void
gui_render_minimap(Gui *gui, Camera *cam)
{
	sprite_render(gui->miniMap, cam);
	sprite_render(gui->miniMapOverlay, cam);
}

void
gui_render_minimap_overlay(Gui *gui, Camera *cam)
{
	static SDL_Rect target_box = {50, 50, GAME_VIEW_WIDTH - 100, GAME_VIEW_HEIGHT - 100};

	// Render map
	Texture *texture = gui->miniMap->textures[0];
	texture_set_alpha(texture, 200);
	texture_render_clip_ex(texture, &target_box, NULL, 0.0, NULL, SDL_FLIP_NONE, cam);
	texture_set_alpha(texture, SDL_ALPHA_OPAQUE);

	// Render map overlay
	texture_render_clip_ex(gui->miniMapOverlay->textures[0], &target_box, NULL, 0.0, NULL, SDL_FLIP_NONE, cam);
}

void
gui_reset(Gui *gui, Camera *cam)
{
	// Clear the minimap
	SDL_SetRenderTarget(cam->renderer, gui->miniMap->textures[0]->texture);
	SDL_SetRenderDrawColor(cam->renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
	SDL_RenderClear(cam->renderer);
	SDL_SetRenderTarget(cam->renderer, NULL);
}

void
gui_log(const char *fmt, ...)
{
	char buffer[LOG_LINES_MAX_LEN];
	uint8_t next_idx;
	char *next_message;
	char tstamp[10];

	va_list args;

	next_idx = (uint8_t)(log_data.head + 1) % log_data.capacity;

	// Increment indexes
	if (log_data.count > 0 && log_data.head == log_data.tail) {
		log_data.tail = (uint8_t)(log_data.tail + 1) % log_data.capacity;
		log_data.count = log_data.capacity;
	} else if (log_data.count < log_data.capacity) {
		log_data.count++;
	}

	next_message = log_data.log[log_data.head];
	log_data.head = next_idx;

	// Build log line
	timestamp(tstamp, 10);
	va_start(args, fmt);
	m_vsprintf(buffer, LOG_LINES_MAX_LEN, fmt, args);
	va_end(args);
	m_sprintf(next_message, log_data.strlen, "%s > %s", tstamp, buffer);
}

void
gui_event_message(const char *fmt, ...)
{
	char *new_message = ec_malloc(sizeof(char) * event_messages.strlen);

	va_list args;
	va_start(args, fmt);
	m_vsprintf(new_message, event_messages.strlen, fmt, args);
	va_end(args);

	if (event_messages.count == event_messages.len) {
		error("To many event messages");
		free(new_message);
		return;
	}
	event_messages.messages[event_messages.count] = new_message;
	event_messages.count++;
}

void
gui_render_tooltip(Gui *gui, Camera *cam)
{
	if (gui->activeTooltip) {
		sprite_render(gui->activeTooltip, cam);
	}
}

void
gui_render_log(Gui *gui, Camera *cam)
{
	static uint8_t last_head = 0;

	sprite_render(gui->bottomFrame, cam);

	// Update log textures
	const uint8_t visible = log_data.count;
	if (log_data.head != last_head) {
		for (uint8_t i = 0; i < visible; ++i) {
			const uint8_t log_index = (uint8_t)((log_data.tail + i) % log_data.capacity);
			Texture *t = gui->log_lines[i];
			texture_load_from_text(t, log_data.log[log_index], C_WHITE, C_BLACK, cam->renderer);
		}
	}
	last_head = log_data.head;

	// Render log textures
	SDL_Rect box = {16, 0, 16, 16};
	for (uint8_t i = 0; i < visible; ++i) {
		Texture *t;
		box.y = 16 + ((LOG_FONT_SIZE + 5) * i);
		t = gui->log_lines[i];
		box.w = t->dim.width;
		box.h = t->dim.height;
		texture_render(t, &box, cam);
	}
}

void
gui_render_event_message(Gui *gui, Camera *cam)
{
	static SDL_Rect box = {0, 0, 150, 50};

	if (timer_started(gui->event_message_timer)
	    && timer_get_ticks(gui->event_message_timer) < EVENT_MESSAGE_DISPLAY_TIME) {
		texture_render(gui->event_message, &box, cam);
		return;
	}

	if (event_messages.count > 0) {
		texture_load_from_text(gui->event_message, event_messages.messages[0], C_WHITE, C_BLACK, cam->renderer);

		box.x = (GAME_VIEW_WIDTH / 2) - (gui->event_message->dim.width / 2);
		box.y = (GAME_VIEW_HEIGHT / 2) - (gui->event_message->dim.height / 2);
		box.w = gui->event_message->dim.width;
		box.h = gui->event_message->dim.height;

		free(event_messages.messages[0]);
		for (size_t i = 1; i < event_messages.count; ++i) {
			event_messages.messages[i - 1] = event_messages.messages[i];
		}
		event_messages.count--;

		texture_render(gui->event_message, &box, cam);
		timer_start(gui->event_message_timer);
	}
}

void
gui_clear_message_log(void)
{
	for (size_t i = 0; i < event_messages.count; ++i) {
		free(event_messages.messages[i]);
	}
	event_messages.count = 0;

	// Don't need to clear the log buffer. Just reset the head/tail
	log_data.head = 0;
	log_data.tail = 0;
	log_data.count = 0;
}

static void
destroy_event_messages(void)
{
	if (event_messages.messages == NULL)
		return;

	for (unsigned int i = 0; i < event_messages.count; ++i) {
		free(event_messages.messages[i]);
	}

	free(event_messages.messages);
	event_messages.messages = NULL;
}

void
gui_destroy(Gui *gui)
{
	destroy_event_messages();

	timer_destroy(gui->event_message_timer);
	texture_destroy(gui->event_message);

	sprite_destroy(gui->bottomFrame);
	sprite_destroy(gui->statsFrame);
	sprite_destroy(gui->miniMap);
	sprite_destroy(gui->miniMapOverlay);
	sprite_destroy(gui->silverKey);
	sprite_destroy(gui->goldKey);

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

	free(gui);
}
