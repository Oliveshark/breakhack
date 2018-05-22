/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
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

#include <stdlib.h>
#include "defines.h"
#include "skillbar.h"
#include "texture.h"
#include "util.h"
#include "sprite.h"
#include "keyboard.h"
#include "texturecache.h"
#include "particle_engine.h"
#include "update_data.h"

static void
load_texture(SkillBar *bar, const char *path, SDL_Renderer *renderer)
{
	Texture *t = texturecache_add(path);
	t->dim.width = 16;
	t->dim.height = 16;

	for (unsigned int i = 0; i < 10; ++i) {
		char buffer[4];
		Sprite *s = sprite_create();
		s->pos = (Position) { i * 32 + 20, 20 };
		s->dim = (Dimension) { 8, 8 };
		s->fixed = true;
		sprite_load_text_texture(s, "GUI/SDS_8x8.ttf", 0, 8, 0);
		m_sprintf(buffer, 4, "%u", i+1 < 10 ? i+1 : 0);
		texture_load_from_text(s->textures[0], buffer, C_YELLOW, C_BLACK, renderer);
		linkedlist_append(&bar->sprites, s);
	}
}

static void
load_countdown_sprites(SkillBar *bar)
{
	for (int i = 0; i < PLAYER_SKILL_COUNT; ++i) {
		Sprite *s = sprite_create();
		sprite_load_text_texture(s, "GUI/SDS_8x8.ttf", 0, 16, 0);
		s->fixed = true;
		s->pos = (Position) { 8 + (32 * i), 8 };
		s->dim = (Dimension) { 16, 16 };
		bar->countdowns[i] = s;
	}
}

SkillBar *
skillbar_create(SDL_Renderer *renderer)
{
	SkillBar *bar = ec_malloc(sizeof(SkillBar));
	bar->sprites = linkedlist_create();
	bar->activationTimer = timer_create();
	bar->skillSparkleTimer = timer_create();
	bar->lastActivation = 0;
	load_texture(bar, "GUI/GUI0.png", renderer);
	load_countdown_sprites(bar);
	return bar;
}

void
skillbar_check_skill_activation(SkillBar *bar, Player *player)
{
	for (int i = 0; i < PLAYER_SKILL_COUNT; ++i) {
		if (!player->skills[i])
			continue;

		if (player->skills[i]->levelcap != player->stats.lvl)
			continue;

		timer_start(bar->skillSparkleTimer);
	}
}

static void
render_frame(Camera *cam)
{
	static SDL_Rect c_top_left	= { 1*16, 10*16, 16, 16 };
	static SDL_Rect c_top_right	= { 3*16, 10*16, 16, 16 };
	static SDL_Rect c_bottom_left	= { 1*16, 12*16, 16, 16 };
	static SDL_Rect c_bottom_right	= { 3*16, 12*16, 16, 16 };

	Texture *t = texturecache_get("GUI/GUI0.png");
	SDL_Rect box = { 0, 0, 16, 16 };

	for (unsigned int i = 0; i < MAP_ROOM_WIDTH; ++i) {
		box.x = i*32;
		box.y = 0;
		texture_render_clip(t, &box, &c_top_left, cam);
		box.y = 16;
		texture_render_clip(t, &box, &c_bottom_left, cam);

		box.x = i*32 + 16;
		box.y = 0;
		texture_render_clip(t, &box, &c_top_right, cam);
		box.y = 16;
		texture_render_clip(t, &box, &c_bottom_right, cam);
	}
}

static void
render_sprites(SkillBar *bar, Camera *cam)
{
	LinkedList *sprites = bar->sprites;

	while (sprites) {
		sprite_render(sprites->data, cam);
		sprites = sprites->next;
	}
}

static void
render_activation_indicator(SkillBar *bar, Camera *cam)
{
	if (!timer_started(bar->activationTimer))
		return;

	unsigned int ticks = timer_get_ticks(bar->activationTimer);
	if (ticks > 500) {
		timer_stop(bar->activationTimer);
		return;
	}

	SDL_Rect square = { (bar->lastActivation - 1) * 32, 0, 32, 32 };
	unsigned int opacity = (unsigned int) ticks/2;
	SDL_SetRenderDrawColor(cam->renderer, 255, 255, 0, (Uint8)(255 - opacity));
	SDL_RenderDrawRect(cam->renderer, &square);
}

static void
render_skill_countdown(SkillBar *bar, int index, unsigned int count, Camera *cam)
{
	char buffer[5];
	Sprite *s = bar->countdowns[index];

	m_sprintf(buffer, 5, "%u", count);
	texture_load_from_text(s->textures[0], buffer, C_WHITE, C_BLACK, cam->renderer);
	sprite_render(s, cam);
}

static void
render_skills(Player *player, Camera *cam)
{
	static SDL_Rect activeSkillBox = { 0, 0, 32, 32 };

	for (int i = 0; i < PLAYER_SKILL_COUNT; ++i) {
		if (!player->skills[i])
			continue;

		Skill *skill = player->skills[i];
		if (skill->icon->dim.width >16)
			skill->icon->pos = (Position) { i * 32, 0 };
		else
			skill->icon->pos = (Position) { 8 + i * 32, 8 };
		sprite_render(skill->icon, cam);

		if (player->skills[i]->active) {
			activeSkillBox.x = i * 32;
			SDL_SetRenderDrawColor(cam->renderer, 0, 0, 255, 100);
			SDL_RenderFillRect(cam->renderer, &activeSkillBox);
		}
	}
}

static void
render_skill_unavailable(SkillBar *bar, Player *player, Camera *cam)
{
	static SDL_Rect unavailableSkillBox = { 0, 0, 32, 32 };

	for (int i = 0; i < PLAYER_SKILL_COUNT; ++i) {
		bool unavailable = false;
		SDL_Color color;

		if (!player->skills[i])
			continue;

		Skill *skill = player->skills[i];
		if (skill->levelcap > player->stats.lvl) {
			unavailable = true;
			color = (SDL_Color) { 0, 0, 0, 220 };
		} else if (skill->resetCountdown
			   || (skill->available && !skill->available(player)))
		{
			unavailable = true;
			color = (SDL_Color) { 255, 0, 0, 70 };
		}

		if (unavailable) {
			unavailableSkillBox.x = i * 32;
			SDL_SetRenderDrawColor(cam->renderer, UNPACK_COLOR(color));
			SDL_RenderFillRect(cam->renderer, &unavailableSkillBox);
			if (skill->resetCountdown) {
				render_skill_countdown(bar,
						       i,
						       skill->resetCountdown,
						       cam);
			}
		}
	}
}

static void
render_skill_sparkles(SkillBar *bar, Player *player)
{
	if (timer_get_ticks(bar->skillSparkleTimer) > 1500) {
		timer_stop(bar->skillSparkleTimer);
		return;
	}

	Position pos = { 0, GAME_VIEW_HEIGHT };
	Dimension dim = { 32, 32 };
	for (int i = 0; i < PLAYER_SKILL_COUNT; ++i) {
		if (!player->skills[i])
			continue;
		else if (player->skills[i]->levelcap != player->stats.lvl)
			continue;

		pos.x += 32 * i;
		particle_engine_sparkle(pos, dim);
	}
}

void
skillbar_render(SkillBar *bar, Player *player, Camera *cam)
{
	render_frame(cam);
	render_skills(player, cam);
	render_sprites(bar, cam);
	render_skill_unavailable(bar, player, cam);
	render_activation_indicator(bar, cam);
	if (timer_started(bar->skillSparkleTimer))
	    render_skill_sparkles(bar, player);
}

void
skillbar_update(SkillBar *bar, UpdateData *data)
{
	Input *input = data->input;

	unsigned int key = 0;
	for (int i = 0; i < 10; ++i) {
		if (!input_key_is_pressed(input, KEY_NUM0 << i))
			continue;
		key = i;
		break;
	}

	if (key != 0) {
		bar->lastActivation = key;
		timer_start(bar->activationTimer);
	}
}

void
skillbar_destroy(SkillBar *bar)
{
	while (bar->sprites)
		sprite_destroy(linkedlist_pop(&bar->sprites));
	for (unsigned int i = 0; i < PLAYER_SKILL_COUNT; ++i)
		if (bar->countdowns[i])
			sprite_destroy(bar->countdowns[i]);
	timer_destroy(bar->activationTimer);
	timer_destroy(bar->skillSparkleTimer);
	free(bar);
}
