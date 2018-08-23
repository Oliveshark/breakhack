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
#include "gui.h"

static void
load_texture(SkillBar *bar, const char *path, SDL_Renderer *renderer)
{
	Texture *t = texturecache_add(path);
	t->dim.width = 16;
	t->dim.height = 16;

	for (unsigned int i = 0; i < 5; ++i) {
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
		sprite_load_text_texture(s, "GUI/SDS_8x8.ttf", 0, 14, 1);
		s->fixed = true;
		s->pos = (Position) { 8 + (32 * i), 8 };
		s->dim = (Dimension) { 16, 16 };
		bar->countdowns[i] = s;
	}
}

static Sprite*
create_frame_sprite(Camera *cam)
{
	static SDL_Rect c_top_left	= { 1*16, 10*16, 16, 16 };
	static SDL_Rect c_top_right	= { 3*16, 10*16, 16, 16 };
	static SDL_Rect c_center_top	= { 2*16, 10*16, 16, 16 };
	static SDL_Rect c_center_bottom	= { 2*16, 12*16, 16, 16 };
	static SDL_Rect c_bottom_left	= { 1*16, 12*16, 16, 16 };
	static SDL_Rect c_bottom_right	= { 3*16, 12*16, 16, 16 };

	Sprite *frame = sprite_create();
	Texture *texture = texture_create();
	texture->dim = (Dimension) { GAME_VIEW_WIDTH, 32 };
	frame->textures[0] = texture;
	frame->destroyTextures = true;
	frame->pos = (Position) { 0, 0 };
	frame->dim = (Dimension) { GAME_VIEW_WIDTH, 32 };
	frame->fixed = true;
	texture_create_blank(texture,
			     SDL_TEXTUREACCESS_TARGET,
			     cam->renderer);

	SDL_SetRenderTarget(cam->renderer, texture->texture);
	SDL_RenderClear(cam->renderer);

	Texture *t = texturecache_get("GUI/GUI0.png");
	SDL_Rect box = { 0, 0, 16, 16 };

	// Render skill squares
	for (Uint32 i = 0; i < 5; ++i) {
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

	// Render inventory box
	box.x = 5 * 32;
	box.y = 0;
	texture_render_clip(t, &box, &c_top_left, cam);
	box.y = 16;
	texture_render_clip(t, &box, &c_bottom_left, cam);
	box.x = 5 * 32 + 16;
	box.y = 0;
	texture_render_clip(t, &box, &c_center_top, cam);
	box.y = 16;
	texture_render_clip(t, &box, &c_center_bottom, cam);

	for (Uint32 i = 6; i < MAP_ROOM_WIDTH - 1; ++i) {
		box.x = i*32;
		box.y = 0;
		texture_render_clip(t, &box, &c_center_top, cam);
		box.y = 16;
		texture_render_clip(t, &box, &c_center_bottom, cam);

		box.x = i * 32 + 16;
		box.y = 0;
		texture_render_clip(t, &box, &c_center_top, cam);
		box.y = 16;
		texture_render_clip(t, &box, &c_center_bottom, cam);
	}

	box.x = (MAP_ROOM_WIDTH - 1) * 32;
	box.y = 0;
	texture_render_clip(t, &box, &c_center_top, cam);
	box.y = 16;
	texture_render_clip(t, &box, &c_center_bottom, cam);
	box.x = (MAP_ROOM_WIDTH - 1) * 32 + 16;
	box.y = 0;
	texture_render_clip(t, &box, &c_top_right, cam);
	box.y = 16;
	texture_render_clip(t, &box, &c_bottom_right, cam);

	SDL_SetRenderTarget(cam->renderer, NULL);

	return frame;
}

SkillBar *
skillbar_create(Camera *cam)
{
	SkillBar *bar = ec_malloc(sizeof(SkillBar));
	bar->sprites = linkedlist_create();
	bar->activationTimer = timer_create();
	bar->skillSparkleTimer = timer_create();
	bar->lastActivation = 0;
	bar->frame = create_frame_sprite(cam);
	bar->artifactDisplayOffset = 5 * 32 + 8;
	load_texture(bar, "GUI/GUI0.png", cam->renderer);
	load_countdown_sprites(bar);

	for (Uint32 i = 0; i < LAST_ARTIFACT_EFFECT; ++i) {
		bar->artifacts[i].aSprite = artifact_sprite_for(i);
		bar->artifacts[i].aSprite->fixed = true;
		bar->artifacts[i].lvl = 0;

		Sprite *lvlSprite = sprite_create();
		lvlSprite->fixed = true;
		lvlSprite->dim = DIM(9, 9);
		sprite_load_text_texture(lvlSprite, "GUI/SDS_8x8.ttf", 0, 9, 0);
		bar->artifacts[i].lvlSprite = lvlSprite;
	}
	return bar;
}

bool
skillbar_check_skill_activation(SkillBar *bar, Player *player)
{
	if (player->stats.lvl == 1)
		return false;

	for (int i = 0; i < PLAYER_SKILL_COUNT; ++i) {
		if (!player->skills[i])
			continue;

		if (player->skills[i]->levelcap != player->stats.lvl)
			continue;

		timer_start(bar->skillSparkleTimer);
	}

	return timer_started(bar->skillSparkleTimer);
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
render_artifacts(SkillBar *bar, Camera *cam)
{
	UNUSED(bar);
	for (size_t i = 0; i < LAST_ARTIFACT_EFFECT; ++i) {
		if (bar->artifacts[i].lvl == 0)
			continue;
		sprite_render(bar->artifacts[i].aSprite, cam);
		sprite_render(bar->artifacts[i].lvlSprite, cam);
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

	if (player->stats.lvl == 1) {
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
		particle_engine_sparkle(pos, dim, C_WHITE, true);
	}
}

void
skillbar_render(SkillBar *bar, Player *player, Camera *cam)
{
	sprite_render(bar->frame, cam);
	render_skills(player, cam);
	render_artifacts(bar, cam);
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

	for (int i = 0; i < 5; ++i) {
		if (!data->player->skills[i])
			continue;
		if (input_modkey_is_pressed(input, KEY_SHIFT_NUM1 << i)) {
			data->gui->activeTooltip = data->player->skills[i]->tooltip;
			return;
		}
	}

	Uint32 key = 0;
	for (int i = 0; i < 5; ++i) {
		if (!input_key_is_pressed(input, KEY_NUM0 << i))
			continue;
		key = i;
		break;
	}

	if (key != 0) {
		bar->lastActivation = key;
		timer_start(bar->activationTimer);
	}

	for (size_t i = 0; i < LAST_ARTIFACT_EFFECT; ++i) {
		if (data->player->equipment.artifacts[i].level == bar->artifacts[i].lvl)
			continue;

		Uint32 origLevel = bar->artifacts[i].lvl;
		bar->artifacts[i].lvl = data->player->equipment.artifacts[i].level;

		char lvl[4];
		m_sprintf(lvl, 4, "%u", bar->artifacts[i].lvl);

		texture_load_from_text(bar->artifacts[i].lvlSprite->textures[0],
				       lvl, C_PURPLE, C_WHITE, data->cam->renderer);

		// Only update position if this is the first pickup
		if (origLevel == 0) {
			bar->artifacts[i].lvlSprite->pos.x = bar->artifactDisplayOffset + 12;
			bar->artifacts[i].lvlSprite->pos.y = 16;
			bar->artifacts[i].aSprite->pos.x = bar->artifactDisplayOffset;
			bar->artifacts[i].aSprite->pos.y = 8;
			bar->artifactDisplayOffset += 32;
		}
	}
}

void
skillbar_reset(SkillBar *bar)
{
	bar->artifactDisplayOffset = 5 * 32 + 8;
}

void
skillbar_destroy(SkillBar *bar)
{
	while (bar->sprites)
		sprite_destroy(linkedlist_pop(&bar->sprites));
	for (Uint32 i = 0; i < PLAYER_SKILL_COUNT; ++i)
		if (bar->countdowns[i])
			sprite_destroy(bar->countdowns[i]);
	for (Uint32 i = 0; i < LAST_ARTIFACT_EFFECT; ++i) {
		sprite_destroy(bar->artifacts[i].aSprite);
		sprite_destroy(bar->artifacts[i].lvlSprite);
	}
	sprite_destroy(bar->frame);
	timer_destroy(bar->activationTimer);
	timer_destroy(bar->skillSparkleTimer);
	free(bar);
}
