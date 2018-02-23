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

#include "defines.h"
#include "skillbar.h"
#include "texture.h"
#include "util.h"
#include "sprite.h"
#include "keyboard.h"

static void
load_texture(SkillBar *bar, const char *path, SDL_Renderer *renderer)
{
	static SDL_Color c_yellow = { 255, 255, 0, 255 };

	Texture *t = texture_create();
	texture_load_from_file(t, path, renderer);
	t->dim.width = 16;
	t->dim.height = 16;
	ht_set(bar->textures, path, t);

	for (unsigned int i = 0; i < 4; ++i) {
		char buffer[4];
		Sprite *s = sprite_create();
		s->pos = (Position) { i * 32 + 20, 20 };
		s->fixed = true;
		sprite_load_text_texture(s, "GUI/SDS_8x8.ttf", 0, 8);
		m_sprintf(buffer, 4, "%u", i+1);
		texture_load_from_text(s->textures[0], buffer, c_yellow, renderer);
		linkedlist_append(&bar->sprites, s);
	}
}

SkillBar *
skillbar_create(SDL_Renderer *renderer)
{
	SkillBar *bar = ec_malloc(sizeof(SkillBar));
	bar->textures = ht_create(10);
	bar->sprites = linkedlist_create();
	bar->activationTimer = timer_create();
	bar->lastActivation = 0;
	load_texture(bar, "GUI/GUI0.png", renderer);
	return bar;
}

static void
render_frame(SkillBar *bar, Camera *cam)
{
	static SDL_Rect c_top_left	= { 1*16, 10*16, 16, 16 };
	static SDL_Rect c_top_right	= { 3*16, 10*16, 16, 16 };
	static SDL_Rect c_bottom_left	= { 1*16, 12*16, 16, 16 };
	static SDL_Rect c_bottom_right	= { 3*16, 12*16, 16, 16 };

	Texture *t = ht_get(bar->textures, "GUI/GUI0.png");
	Position p = { 0, 0 };

	for (unsigned int i = 0; i < MAP_ROOM_WIDTH; ++i) {
		p.x = i*32;
		p.y = 0;
		texture_render_clip(t, &p, &c_top_left, cam);
		p.y = 16;
		texture_render_clip(t, &p, &c_bottom_left, cam);

		p.x = i*32 + 16;
		p.y = 0;
		texture_render_clip(t, &p, &c_top_right, cam);
		p.y = 16;
		texture_render_clip(t, &p, &c_bottom_right, cam);
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

void
skillbar_render(SkillBar *bar, Camera *cam)
{
	render_frame(bar, cam);
	render_sprites(bar, cam);
	render_activation_indicator(bar, cam);
}

void
skillbar_handle_event(SkillBar *bar, SDL_Event *event)
{
	if (event->type != SDL_KEYDOWN)
		return;

	unsigned int key = 0;
	if (keyboard_press(SDLK_1, event))
		key = 1;
	else if (keyboard_press(SDLK_2, event))
		key = 2;
	else if (keyboard_press(SDLK_3, event))
		key = 3;
	else if (keyboard_press(SDLK_4, event))
		key = 4;

	if (key != 0) {
		bar->lastActivation = key;
		timer_start(bar->activationTimer);
	}
}

void
skillbar_destroy(SkillBar *bar)
{
	ht_destroy_custom(bar->textures, (void (*)(void *)) texture_destroy);
	while (bar->sprites)
		sprite_destroy(linkedlist_pop(&bar->sprites));
	free(bar);
}
