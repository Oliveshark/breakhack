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

#include "screen.h"
#include "util.h"
#include "hiscore.h"

static Screen *
screen_create(void)
{
	Screen *screen = ec_malloc(sizeof(Screen));
	screen->textures = linkedlist_create();
	screen->sprites = linkedlist_create();
	return screen;
}

static Sprite *
credit_txt(const char *msg, SDL_Color color, int x, int y, SDL_Renderer *renderer)
{
	Sprite *s = sprite_create();
	sprite_load_text_texture(s, "GUI/SDS_8x8.ttf", 0, 14, 1);
	texture_load_from_text(s->textures[0], msg, color, C_BLACK, renderer);
	s->pos = (Position) { x, y };
	s->fixed = true;
	s->dim = s->textures[0]->dim;
	return s;
}

static Sprite *
score_txt(const char *msg, SDL_Color color, int x, int y, SDL_Renderer *renderer)
{
	Sprite *s = sprite_create();
	sprite_load_text_texture(s, "GUI/SDS_8x8.ttf", 0, 10, 1);
	texture_load_from_text(s->textures[0], msg, color, C_BLACK, renderer);
	s->pos = (Position) { x, y };
	s->fixed = true;
	s->dim = s->textures[0]->dim;
	return s;
}

Screen *
screen_create_credits(SDL_Renderer *renderer)
{
	int x = 20;
	int y = 50;
	unsigned int columnOffset = 160;

	Screen *screen = screen_create();
	linkedlist_push(&screen->sprites, credit_txt("- Game -", C_BLUE, x, y, renderer));
	y += 30;
	linkedlist_push(&screen->sprites, credit_txt("Code:", C_YELLOW, x, y, renderer));
	linkedlist_push(&screen->sprites, credit_txt("Linus Probert", C_WHITE, x + columnOffset, y, renderer));
	y += 20;
	linkedlist_push(&screen->sprites, credit_txt("liquidityc.github.io", C_WHITE, x + columnOffset, y, renderer));
	y += 20;
	linkedlist_push(&screen->sprites, credit_txt("@LiquidityC", C_WHITE, x + columnOffset, y, renderer));

	y += 60;
	linkedlist_push(&screen->sprites, credit_txt(" - Graphics -", C_BLUE, x, y, renderer));
	y += 30;
	linkedlist_push(&screen->sprites, credit_txt("Palette:", C_YELLOW, x, y, renderer));
	linkedlist_push(&screen->sprites, credit_txt("DawnBringer", C_WHITE, x + columnOffset, y, renderer));

	y += 60;
	linkedlist_push(&screen->sprites, credit_txt(" - Music and Sound -", C_BLUE, x, y, renderer));
	y += 30;
	linkedlist_push(&screen->sprites, credit_txt("Music:", C_YELLOW, x, y, renderer));
	linkedlist_push(&screen->sprites, credit_txt("Eric Matyas", C_WHITE, x + columnOffset, y, renderer));
	y += 20;
	linkedlist_push(&screen->sprites, credit_txt("www.soundimage.org", C_WHITE, x + columnOffset, y, renderer));
	y += 30;
	linkedlist_push(&screen->sprites, credit_txt("Sound:", C_YELLOW, x, y, renderer));
	linkedlist_push(&screen->sprites, credit_txt("Eric Matyas", C_WHITE, x + columnOffset, y, renderer));
	y += 20;
	linkedlist_push(&screen->sprites, credit_txt("www.soundimage.org", C_WHITE, x + columnOffset, y, renderer));
	y += 30;
	linkedlist_push(&screen->sprites, credit_txt("ArtisticDuded", C_WHITE, x + columnOffset, y, renderer));
	y += 20;
	linkedlist_push(&screen->sprites, credit_txt("opengameart.org/users/artisticdude", C_WHITE, x + columnOffset, y, renderer));
	return screen;
}

Screen *
screen_create_hiscore(SDL_Renderer *renderer)
{
	Screen *screen = screen_create();
	int y = 40;
	int dateCol = 50;
	int goldCol = 250;
	int lvlCol = 400;
	int dlvlCol = 550;

	linkedlist_push(&screen->sprites, score_txt("Date",
						    C_GREEN,
						    dateCol,
						    y,
						    renderer));
	linkedlist_push(&screen->sprites, score_txt("Gold",
						    C_GREEN,
						    goldCol,
						    y,
						    renderer));
	linkedlist_push(&screen->sprites, score_txt("Level",
						    C_GREEN,
						    lvlCol,
						    y,
						    renderer));
	linkedlist_push(&screen->sprites, score_txt("Depth",
						    C_GREEN,
						    dlvlCol,
						    y,
						    renderer));

	LinkedList *scores = hiscore_get_top10();
	while (scores) {
		y += 30;
		HiScore *score = linkedlist_pop(&scores);
		char content[80];
		m_sprintf(content, 80, "%s", score->timestamp);
		linkedlist_push(&screen->sprites,
				score_txt(content,
					  C_WHITE,
					  dateCol,
					  y,
					  renderer));
		m_sprintf(content, 80, "%.2lf", score->gold);
		linkedlist_push(&screen->sprites,
				score_txt(content,
					  C_YELLOW,
					  goldCol,
					  y,
					  renderer));
		m_sprintf(content, 80, "%d", score->playerLevel);
		linkedlist_push(&screen->sprites,
				score_txt(content,
					  C_BLUE,
					  lvlCol,
					  y,
					  renderer));
		m_sprintf(content, 80, "%d", score->dungeonLevel);
		linkedlist_push(&screen->sprites,
				score_txt(content,
					  C_RED,
					  dlvlCol,
					  y,
					  renderer));
		hiscore_destroy(score);
	}
	return screen;
}

void
screen_render(Screen *screen, Camera *cam)
{
	LinkedList *textures = screen->textures;
	while (textures) {
		texture_render(textures->data, NULL, cam);
		textures = textures->next;
	}
	LinkedList *sprites = screen->sprites;
	while (sprites) {
		sprite_render(sprites->data, cam);
		sprites = sprites->next;
	}
}

void
screen_destroy(Screen *screen)
{
	while (screen->textures)
		texture_destroy(linkedlist_pop(&screen->textures));
	while (screen->sprites)
		sprite_destroy(linkedlist_pop(&screen->sprites));
	free(screen);
}
