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
#pragma once

#include "texture.h"
#include "linkedlist.h"
#include "sprite.h"
#include "camera.h"

typedef struct Screen {
	LinkedList *sprites;
	LinkedList *textures;
} Screen;

Screen *
screen_create_credits(SDL_Renderer*);

Screen *
screen_create_hiscore(SDL_Renderer*);

Screen *
screen_create_characterselect(SDL_Renderer*);

void
screen_render(Screen *screen, Camera *cam);

void
screen_destroy(Screen *screen);
