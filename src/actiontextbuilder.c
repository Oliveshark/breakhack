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

#include <assert.h>
#include "actiontextbuilder.h"
#include "util.h"
#include "linkedlist.h"

static SDL_Renderer *gRenderer = NULL;
static LinkedList *actiontexts;

void
actiontextbuilder_init(SDL_Renderer *renderer)
{
	gRenderer = renderer;
	actiontexts = linkedlist_create();
}

void
actiontextbuilder_create_text(const char *msg, SDL_Color color, Position *p)
{
	assert (gRenderer != NULL);
	Sprite *sprite = sprite_create();
	sprite->pos = *p;
	sprite_load_text_texture(sprite, "GUI/SDS_8x8.ttf", 0, 11, 1);
	texture_load_from_text(sprite->textures[0], msg, color, C_BLACK, gRenderer);
	sprite->dim = sprite->textures[0]->dim;
	linkedlist_append(&actiontexts, actiontext_create(sprite));
}

void
actiontextbuilder_update(UpdateData *data)
{
	LinkedList *remaining = linkedlist_create();
	while (actiontexts) {
		ActionText *text = linkedlist_pop(&actiontexts);
		if (text->dead) {
			actiontext_destroy(text);
		} else {
			actiontext_update(text, data);
			linkedlist_append(&remaining, text);
		}
	}
	actiontexts = remaining;
}

void
actiontextbuilder_render(Camera *cam)
{
	LinkedList *actiontext = actiontexts;
	while (actiontext) {
		actiontext_render(actiontext->data, cam);
		actiontext = actiontext->next;
	}
}

void
actiontextbuilder_close(void)
{
	while (actiontexts)
		actiontext_destroy(linkedlist_pop(&actiontexts));
}
