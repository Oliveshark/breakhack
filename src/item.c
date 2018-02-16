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

Item *
item_create(void)
{
	Item *item = ec_malloc(sizeof(Item));
	item->sprite = NULL;
	item->collected = false;
	m_strcpy(item->label, 50, "");
	item->value = 0.0;
	item->items = NULL;
	item->effect = NULL;
	return item;
}

void
item_render(Item *item, Camera *cam)
{
	sprite_render(item->sprite, cam);
}

void
item_collected(Item *item, Player *player)
{
	LinkedList *items = item->items;

	while (items != NULL) {
		Item *subitem = items->data;
		items = items->next;
		item_collected(subitem, player);
	}

	if (item->effect != NULL)
		item->effect(item, player);

	item->collected = true;
}

void
item_destroy(Item *item)
{
	if (item->sprite)
		sprite_destroy(item->sprite);

	while (item->items != NULL)
		item_destroy(linkedlist_pop(&item->items));

	free(item);
}
