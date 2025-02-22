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
#pragma once

#include <stdlib.h>
#include "linkedlist.h"
#include "player.h"

typedef struct HiScore {
	char *timestamp;
	double gold;
	unsigned int playerLevel;
	unsigned int dungeonLevel;
	LinkedList *artifacts;
} HiScore;

void
hiscore_init(void);

void
hiscore_register(Player *p, unsigned int dungeonLevel);

LinkedList *
hiscore_get_top10(void);

double
hiscore_get_top_gold(void);

void
hiscore_close(void);

void
hiscore_destroy(HiScore *);
