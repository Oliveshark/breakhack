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

#ifndef MONSTER_H_
#define	MONSTER_H_

#include <SDL2/SDL.h>
#include "sprite.h"
#include "stats.h"
#include "actiontext.h"

typedef enum { PASSIVE, AGRESSIVE, SCARED } StateType;

typedef struct {
	StateType current;
	StateType normal;
	StateType challenge;
} State;

typedef struct Monster_t {
	char *label;
	char *lclabel;
	Sprite *sprite;
	ActionText *hitText;
	ActionText *missText;
	Stats stats;
	State state;
	unsigned int steps;
} Monster;

Monster* monster_create(SDL_Renderer*);

void
monster_update_pos(Monster*, Position);

bool
monster_move(Monster*, RoomMatrix*);

void
monster_render(Monster*, Camera*);

void
monster_hit(Monster*, unsigned int dmg);

void
monster_update_stats_for_level(Monster*, unsigned int level);

void
monster_drop_loot(Monster*, Map*);

void
monster_destroy(Monster*);

#endif // MONSTER_H_
