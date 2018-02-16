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

#include <stdlib.h>
#include "util.h"
#include "gui_button.h"
#include "collisions.h"

GuiButton *
gui_button_create(SDL_Rect area, void (*event)(void*), void *usrdata)
{
	GuiButton *button = ec_malloc(sizeof(GuiButton));
	button->area = area;
	button->hover = false;
	button->usrdata = usrdata;
	button->event = event;
	return button;
}

void
gui_button_check_pointer(GuiButton *button, Pointer *pointer)
{
	button->hover = position_in_rect(&pointer->sprite->pos, &button->area);
	pointer_toggle_clickable_pointer(pointer, button->hover);
}

void
gui_button_handle_event(GuiButton *button, SDL_Event *event)
{
	if (event->type == SDL_MOUSEBUTTONDOWN) {

		if (event->button.button != SDL_BUTTON_LEFT)
			return;

		Position p = { event->button.x, event->button.y };
		if (position_in_rect(&p, &button->area) && button->event)
			button->event(button->usrdata);

	} else if (event->type == SDL_MOUSEMOTION) {
		Position p = { event->motion.x, event->motion.y };
		button->hover = position_in_rect(&p, &button->area);
	}
}

void
gui_button_destroy(GuiButton *button)
{
	free(button);
}
