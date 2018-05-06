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

#ifndef TEXTURECACHE_H_
#define TEXTURECACHE_H_

#include <SDL.h>
#include "texture.h"

void
texturecache_init(SDL_Renderer*);

Texture*
texturecache_add(const char *path);

Texture*
texturecache_get(const char *path);

void
texturecache_close(void);

#endif // TEXTURECACHE_H_

