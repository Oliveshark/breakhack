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
#pragma once

#include <SDL3/SDL.h>
#include "sprite.h"
#include "stats.h"
#include "actiontext.h"
#include "camera.h"
#include "skill.h"
#include "linkedlist.h"
#include "input.h"
#include "artifact.h"

#define PLAYER_SKILL_COUNT 5

// Foward declare
typedef struct UpdateData UpdateData;
typedef struct Animation Animation;

typedef enum PlayerClass { ENGINEER, MAGE, PALADIN, ROGUE, WARRIOR } class_t;
typedef enum PlayerState { ALIVE, DEAD } state_t;

typedef struct PlayerStatData {
	unsigned int total_steps;
	unsigned int steps;
	unsigned int hits;
	unsigned int kills;
	unsigned int misses;
} PlayerStatData;

typedef struct ExperienceDatat {
	unsigned int previousLevel;
	unsigned int current;
	unsigned int nextLevel;
	unsigned int level;
} ExperienceData;

typedef struct ArtifactData {
	const char *name;
	const char *desc;
	Uint32 level;
} ArtifactData;

typedef struct PlayerEquipment {
	ArtifactData artifacts[LAST_ARTIFACT_EFFECT];
	bool hasArtifacts;
	Uint32 keys;
} PlayerEquipment;

typedef struct PlayerStateData {
	bool shopOwnerKiller;
} PlayerStateData;

typedef struct Player {
	Sprite *sprite;
	Stats stats;
	unsigned int daggers;
	LinkedList *projectiles;
	unsigned int xp;
	double gold;
	PlayerStatData stat_data;
	unsigned int potion_sips;
	unsigned int phase_count;
	class_t class;
	state_t state;
	Skill *skills[PLAYER_SKILL_COUNT];
	Timer *animationTimer;
	Animation *swordAnimation;
	PlayerEquipment equipment;
	PlayerStateData stateData;
} Player;

Player*
player_create(class_t, Camera*);

void
player_reset_on_levelchange(Player *player);

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
player_update(struct UpdateData *);

void
player_render(Player*, Camera*);

void
player_render_toplayer(Player*, Camera*);

void
player_turn(Player*, Vector2d *dir);

void
player_destroy(Player*);

bool
player_turn_over(Player*);

void
player_levelup(Player*);

void
player_set_level(Player*, Uint8 level);

Uint32
player_has_artifact(Player *, MagicalEffect);

void
player_add_artifact(Player*, Artifact*);

void
player_set_falling(Player*);
