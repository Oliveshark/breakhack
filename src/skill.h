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

#ifndef SKILL_H_
#define	SKILL_H_

#include <stdbool.h>
#include "player.h"
#include "roommatrix.h"
#include "sprite.h"
#include "vector2d.h"

enum SkillType {
	FLURRY
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
	bool active;
	bool (*use)(struct Skill_t*, SkillData*);
} Skill;

Skill*
skill_create(enum SkillType);

void
skill_destroy(Skill*);

#endif // SKILL_H_
