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

#ifndef ITEM_H_
#define ITEM_H_

#include <stdbool.h>

#include "sprite.h"
#include "position.h"
#include "player.h"
#include "camera.h"
#include "linkedlist.h"

/**
 * \brief A game item struct
 */
typedef struct Item_t {
	Sprite *sprite;                            /**< The item sprite */
	LinkedList *subsprites;                    /**< A list of sub-sprites (labels) */
	bool collected;                            /**< If the item has been collected */
	bool openable;                             /**< Can the item be opened? */
	bool opened;                               /**< Has the item been opened? */
	char label[50];                            /**< The Item label */
	double price;                              /**< The item price (for vendor) */
	double value;                              /**< Value, the item value. Eg. Gold */
	LinkedList *items;                         /**< Sub items */
	void (*effect)(struct Item_t *, Player *); /**< Item effect callback */
} Item;

Item *item_create(void);

void item_collected(Item *, Player *);

void item_update(Item *, UpdateData *);

void item_render(Item *, Camera *);

void item_destroy(Item *);

#endif // ITEM_H_
