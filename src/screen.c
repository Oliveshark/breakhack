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

static Screen *
screen_create(void)
{
	Screen *screen = ec_malloc(sizeof(Screen));
	screen->textures = linkedlist_create();
	screen->sprites = linkedlist_create();
	return screen;
}

static Sprite *
create_text_sprite(const char *msg, int x, int y, SDL_Renderer *renderer)
{
	Sprite *s = sprite_create();
	sprite_load_text_texture(s, "GUI/SDS_8x8.ttf", 0, 14, 1);
	texture_load_from_text(s->textures[0], msg, C_BLUE, C_WHITE, renderer);
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
	linkedlist_push(&screen->sprites, create_text_sprite("- Game -", x, y, renderer));
	y += 30;
	linkedlist_push(&screen->sprites, create_text_sprite("Code:", x, y, renderer));
	linkedlist_push(&screen->sprites, create_text_sprite("Linus Probert", x + columnOffset, y, renderer));
	y += 20;
	linkedlist_push(&screen->sprites, create_text_sprite("liquidityc.github.io", x + columnOffset, y, renderer));
	y += 20;
	linkedlist_push(&screen->sprites, create_text_sprite("@LiquidityC", x + columnOffset, y, renderer));

	y += 60;
	linkedlist_push(&screen->sprites, create_text_sprite(" - Graphics -", x, y, renderer));
	y += 30;
	linkedlist_push(&screen->sprites, create_text_sprite("Palette:", x, y, renderer));
	linkedlist_push(&screen->sprites, create_text_sprite("DawnBringer", x + columnOffset, y, renderer));

	y += 60;
	linkedlist_push(&screen->sprites, create_text_sprite(" - Music and Sound -", x, y, renderer));
	y += 30;
	linkedlist_push(&screen->sprites, create_text_sprite("Music:", x, y, renderer));
	linkedlist_push(&screen->sprites, create_text_sprite("Eric Matyas", x + columnOffset, y, renderer));
	y += 20;
	linkedlist_push(&screen->sprites, create_text_sprite("www.soundimage.org", x + columnOffset, y, renderer));
	y += 30;
	linkedlist_push(&screen->sprites, create_text_sprite("Sound:", x, y, renderer));
	linkedlist_push(&screen->sprites, create_text_sprite("Eric Matyas", x + columnOffset, y, renderer));
	y += 20;
	linkedlist_push(&screen->sprites, create_text_sprite("www.soundimage.org", x + columnOffset, y, renderer));
	y += 30;
	linkedlist_push(&screen->sprites, create_text_sprite("ArtisticDuded", x + columnOffset, y, renderer));
	y += 20;
	linkedlist_push(&screen->sprites, create_text_sprite("opengameart.org/users/artisticdude", x + columnOffset, y, renderer));
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
