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

#ifndef POINTER_H_
#define	POINTER_H_

#include <SDL3/SDL.h>
#include "sprite.h"
#include "camera.h"
#include "input.h"

typedef struct Pointer_t {
	Sprite *sprite;
} Pointer;

Pointer *
pointer_create(SDL_Renderer *renderer);

void
pointer_handle_input(Pointer*, Input *);

void
pointer_toggle_clickable_pointer(Pointer *, bool clickable);

void
pointer_render(Pointer*, Camera*);

void
pointer_destroy(Pointer*);

#endif // POINTER_H_
