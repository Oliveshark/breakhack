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

#ifndef ITEMBUILDER_H_
#define ITEMBUILDER_H_

#include "item.h"

typedef struct {
	SDL_Renderer *renderer;
} ItemBuilder;

typedef enum {
	COPPER,
	SILVER,
	GOLD,
	PLATINUM,
	TREASURE_COUNT
} Treasure;

typedef enum {
	HEALTH,
	DAGGER,
	FLESH,
	TREASURE,
	ITEM_COUNT
} ItemKey;

void
item_builder_init(SDL_Renderer *);

Item *
item_builder_build_item(ItemKey key, int level);

Item *
item_builder_build_sack(void);

Item *
item_builder_build_container(const char *path0, const char *path1, SDL_Rect clip);

Item *
item_builder_build_treasure(Treasure type, double goldAmt);

Item *
item_builder_build_key(unsigned int type);

void
item_builder_close(void);

#endif // ITEMBUILDER_H_
