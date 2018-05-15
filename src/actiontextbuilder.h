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

#ifndef ACTIONTEXTBUILDER_H_
#define	ACTIONTEXTBUILDER_H_

#include <SDL.h>
#include "actiontext.h"
#include "camera.h"
#include "update_data.h"

void
actiontextbuilder_init(SDL_Renderer*);

void
actiontextbuilder_update(UpdateData*);

void
actiontextbuilder_render(Camera*);

void
actiontextbuilder_create_text(const char *msg, SDL_Color, Position*);

void
actiontextbuilder_close(void);

#endif // ACTIONTEXTBUILDER_H_

