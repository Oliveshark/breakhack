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

#include "SDL.h"
#include "hashtable.h"
#include "linkedlist.h"
#include "camera.h"

typedef struct SkillBar_t {
	Hashtable *textures;
	LinkedList *sprites;
} SkillBar;

SkillBar *
skillbar_create(SDL_Renderer*);

void
skillbar_render(SkillBar*, Camera*);

void
skillbar_destroy(SkillBar*);

#endif // SKILLBAR_H_
