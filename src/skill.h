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

#ifndef SKILL_H_
#define	SKILL_H_

#include <stdbool.h>
#include "roommatrix.h"
#include "sprite.h"
#include "vector2d.h"
#include "tooltip.h"

// Forward declaration
typedef struct Player Player;

enum SkillType {
	FLURRY,
	BASH,
	CHARGE,
	DAGGER_THROW,
	SIP_HEALTH,
	BACKSTAB,
	TRIP,
	PHASE,
	BLINK,
	ERUPT,
	VAMPIRIC_BLOW
};

typedef struct SkillData_t {
	Player *player;
	RoomMatrix *matrix;
	Vector2d direction;
} SkillData;

typedef struct Skill_t {
	char label[20];
	Sprite *icon;
	unsigned int resetTime;
	unsigned int resetCountdown;
	unsigned int levelcap;
	bool actionRequired;
	bool instantUse;
	bool active;
	bool (*available)(Player*);
	bool (*use)(struct Skill_t*, SkillData*);
	Tooltip *tooltip;
} Skill;

Skill*
skill_create(enum SkillType, Camera *cam);

void
skill_destroy(Skill*);

#endif // SKILL_H_
