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
#ifndef SAVE_H_
#define	SAVE_H_

#include <stdbool.h>
#include "player.h"
#include "artifact.h"

typedef struct Save {
	int seed;
	bool quickGame;
	bool arcadeGame;
	unsigned int map_level;
	unsigned int player_daggers;
	unsigned int player_xp;
	unsigned int player_potion_sips;
	unsigned int potion_sips;
	Stats player_stats;
	PlayerStatData player_player_stats;
	double player_gold;
	PlayerStateData player_state;
	class_t player_class;
	PlayerEquipment player_equipment;
} Save;

void
save_init(void);

void
save_load(void);

const Save *
save_get(void);

bool
save_exists(void);

void
save_save(unsigned int seed,
	  unsigned int map_level,
	  bool quickGame,
	  bool arcadeGame,
	  Player *player);

void
save_clear(void);

void
save_close(void);

#endif // SAVE_H_
