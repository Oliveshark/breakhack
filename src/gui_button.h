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

#ifndef GUI_BUTTON_H_
#define	GUI_BUTTON_H_

#include <SDL3/SDL.h>
#include "pointer.h"
#include "sprite.h"
#include "linkedlist.h"
#include "camera.h"
#include "input.h"

typedef struct GuiButton {
	SDL_Rect area;
	bool hover;
	void *usrdata;
	void (*event)(void*);
} GuiButton;

GuiButton *
gui_button_create(SDL_Rect, void (*)(void*), void*);

void
gui_button_check_pointer(GuiButton*, Pointer*);

void
gui_button_update(GuiButton*, Input*);

void
gui_button_destroy(GuiButton*);

#endif // GUI_BUTTON_H_
