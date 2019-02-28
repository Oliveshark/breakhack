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
#include "item.h"
#include "util.h"
#include "mixer.h"
#include "gui.h"
#include "actiontextbuilder.h"

Item *
item_create(void)
{
	Item *item = ec_malloc(sizeof(Item));
	item->sprite = NULL;
	item->subsprites = NULL;
	item->collected = false;
	item->openable = false;
	item->opened = false;
	m_strcpy(item->label, 50, "");
	item->price = 0.0;
	item->value = 0.0;
	item->items = NULL;
	item->effect = NULL;
	return item;
}

void
item_render(Item *item, Camera *cam)
{
	sprite_render(item->sprite, cam);

	LinkedList *subsprites = item->subsprites;
	while (subsprites != NULL) {
		Sprite *sprite = subsprites->data;
		sprite->pos = item->sprite->pos;
		sprite->pos.x + 15 - sprite->dim.width / 2;
		sprite_render(sprite, cam);
		subsprites = subsprites->next;
	}
}

void
item_collected(Item *item, Player *player)
{
	if (item->price > player->gold){
		gui_log("You don't have enough gold to buy %s", item->label);
		return;
	}

	if (item->collected || item->opened)
		return;

	if (!item->openable) {
		item->collected = true;
	} else {
		mixer_play_effect(CHEST_OPEN);
		gui_log("You open a container");
		item->opened = true;
		item->sprite->texture_index = 1;
		if (!item->items)
			gui_log("You find nothing inside");
	}

	if (item->price) {
	    player->gold -= item->price;
	    char costLabel[10];
	    m_sprintf(costLabel, 10, "-$%.0f", item->price);
	    actiontextbuilder_create_text(costLabel, C_YELLOW, &player->sprite->pos);
	}

	if (item->effect != NULL)
		item->effect(item, player);

	while (item->items) {
		Item *subitem = linkedlist_pop(&item->items);
		item_collected(subitem, player);
		item_destroy(subitem);
	}
}

void
item_destroy(Item *item)
{
	if (item->sprite)
		sprite_destroy(item->sprite);

	while (item->subsprites != NULL)
		sprite_destroy(linkedlist_pop(&item->subsprites));

	while (item->items != NULL)
		item_destroy(linkedlist_pop(&item->items));

	free(item);
}
