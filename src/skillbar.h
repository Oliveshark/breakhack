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

#ifndef SKILLBAR_H_
#define	SKILLBAR_H_

#include <SDL.h>
#include "linkedlist.h"
#include "camera.h"
#include "timer.h"
#include "player.h"
#include "input.h"

struct UpdateData;

typedef struct SkillBar_t {
	LinkedList *sprites;
	Sprite *countdowns[PLAYER_SKILL_COUNT];
	Timer *activationTimer;
	Timer *skillSparkleTimer;
	unsigned int lastActivation;
} SkillBar;

SkillBar *
skillbar_create(SDL_Renderer*);

void
skillbar_check_skill_activation(SkillBar*, Player*);

void
skillbar_render(SkillBar*, Player*, Camera*);

void
skillbar_update(SkillBar*, struct UpdateData*);

void
skillbar_destroy(SkillBar*);

#endif // SKILLBAR_H_
