/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2026  Linus Probert <linus.probert@gmail.com>
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
#include "camera.h"
#include "input.h"

void text_input_init(SDL_Window *window, SDL_Renderer *renderer, const char *title, const char *current);

void text_input_update(Input *input);

bool text_input_is_confirmed(void);

const char *text_input_get_value(void);

void text_input_render(Camera *cam);

void text_input_close(SDL_Window *window);
