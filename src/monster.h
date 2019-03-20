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

#include <SDL.h>
#include "sprite.h"
#include "stats.h"
#include "actiontext.h"
#include "player.h"
#include "linkedlist.h"
#include "doorlocktype.h"

struct UpdateData;

typedef enum {
	PACIFIST,
	NORMAL,
	HOSTILE,
	GUERILLA,
	COWARD,
	SENTINEL,
	FIRE_DEMON,
	SORCERER,
	ASSASSIN
} MonsterBehaviour;

typedef enum {
	PASSIVE,
	AGRESSIVE,
	SCARED,
	STATIONARY,
	SLEEPING,
	SCANNING,
	STUNNED
} StateType;

typedef struct State {
	StateType current;
	StateType last;
	unsigned int stepsSinceChange;
	Uint8 forceCount;
} State;

typedef struct MonsterStateIndicator {
	Sprite *sprite;
	bool shownOnPlayerRoomEnter;
	int displayCount;
} MonsterStateIndicator;

typedef struct MonsterItems {
	enum DoorLockType keyType;
} MonsterItems;

typedef struct Monster {
	char *label;
	char *lclabel;
	Sprite *sprite;
	Stats stats;
	State state;
	MonsterStateIndicator stateIndicator;
	MonsterBehaviour behaviour;
	MonsterItems items;
	unsigned int steps;
	bool boss;
	bool bloodlust;
} Monster;

Monster* monster_create(void);

void
monster_update_pos(Monster*, Position);

bool
monster_move(Monster*, RoomMatrix*, Map*);

void
monster_render(Monster*, Camera*);

void
monster_render_top_layer(Monster*, RoomMatrix*, Camera*);

void
monster_hit(Monster*, unsigned int dmg);

void
monster_update_stats_for_level(Monster*, unsigned int level);

void
monster_update(Monster*, struct UpdateData*);

void
monster_drop_loot(Monster*, Map*, Player*);

void
monster_set_behaviour(Monster *, MonsterBehaviour behaviour);

void
monster_set_state(Monster *m, StateType state, Uint8 forceCount);

void
monster_push(Monster *, Player *, RoomMatrix*, Vector2d dir);

void
monster_reset_steps(Monster *m);

void
monster_set_bloodlust(Monster*, bool bloodlust);

void
monster_destroy(Monster*);

#endif // MONSTER_H_
