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

#ifndef PLAYER_H_
#define	PLAYER_H_

#include <SDL.h>
#include "sprite.h"
#include "stats.h"
#include "actiontext.h"
#include "camera.h"
#include "skill.h"

#define PLAYER_SKILL_COUNT 5

enum PlayerClass { ENGINEER, MAGE, PALADIN, ROGUE, WARRIOR };
typedef enum PlayerClass class_t;

typedef struct ExperienceData_t {
	unsigned int previousLevel;
	unsigned int current;
	unsigned int nextLevel;
	unsigned int level;
} ExperienceData;

typedef struct Player_t {
	Sprite *sprite;
	ActionText *hitText;
	ActionText *missText;
	Stats stats;
	unsigned int xp;
	unsigned int total_steps;
	unsigned int steps;
	unsigned int hits;
	unsigned int kills;
	unsigned int misses;
	double gold;
	unsigned int potion_sips;
	class_t class;
	Skill *skills[PLAYER_SKILL_COUNT];
	void (*handle_event)(struct Player_t*, RoomMatrix*, SDL_Event*);
} Player;

Player*
player_create(class_t, SDL_Renderer*);

ExperienceData
player_get_xp_data(Player*);

void
player_monster_kill_check(Player*, Monster*);

void
player_sip_health(Player*);

void
player_hit(Player*, unsigned int dmg);

void
player_reset_steps(Player*);

void
player_render(Player*, Camera*);

void
player_destroy(Player*);

#endif // PLAYER_H_
