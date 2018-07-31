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
#include <stdbool.h>
#include "menu.h"
#include "util.h"
#include "sprite.h"
#include "defines.h"
#include "gui_button.h"
#include "keyboard.h"
#include "mixer.h"
#include "collisions.h"

typedef struct MenuItems {
	Sprite *sprite;
	Sprite *hsprite;
	GuiButton *button;
} MenuItem;

Menu *
menu_create(void)
{
	Menu *menu = ec_malloc(sizeof(Menu));
	menu->items = linkedlist_create();
	menu->selected = 0;
	return menu;
}

static bool
handle_keyboard_input(Menu *m, Input *input)
{
	int lastSelect = -1;

	if (input_key_is_pressed(input, KEY_UP)) {
		lastSelect = m->selected;
		m->selected--;
	} else if (input_key_is_pressed(input, KEY_DOWN)) {
		lastSelect = m->selected;
		m->selected++;
	} else if (input_key_is_pressed(input, KEY_ENTER)) {
		MenuItem *item = linkedlist_get(&m->items, m->selected);
		if (item->button->event)
			item->button->event(item->button->usrdata);
		return true;
	} else {
		return false;
	}
	m->selected = m->selected % linkedlist_size(m->items);

	if (lastSelect != -1)
		mixer_play_effect(CLICK);

	((MenuItem*) linkedlist_get(&m->items, lastSelect))->button->hover = false;
	((MenuItem*) linkedlist_get(&m->items, m->selected))->button->hover = true;

	return false;
}

static void
handle_mouse_motion(Menu *m, Input *input)
{
	if (!input_mouse_moved(input))
		return;

	Position p = { input->mouseX, input->mouseY };

	LinkedList *items = m->items;
	int index = 0;
	while (items) {
		MenuItem *item = items->data;
		items = items->next;

		if (position_in_rect(&p, &item->button->area)) {
			m->selected = index;
			break;
		}
		index++;
	}

}

void
menu_update(Menu *m, Input *input)
{
	if (handle_keyboard_input(m, input)) {
		return;
	}
	handle_mouse_motion(m, input);

	LinkedList *items = m->items;
	while (items) {
		MenuItem *item = items->data;
		items = items->next;

		Position p = { input->mouseX, input->mouseY };
		if (position_in_rect(&p, &item->button->area)
		    && input_mousebutton_is_pressed(input, MBUTTON_LEFT))
		{
			item->button->event(NULL);
			return;
		}
	}
}

void
menu_item_add(Menu *m, Sprite *s1, Sprite *s2, void (*event)(void*))
{
	MenuItem *item = ec_malloc(sizeof(MenuItem));
	item->sprite = s1;
	item->hsprite = s2;

	SDL_Rect area = {
		item->sprite->pos.x,
		item->sprite->pos.y,
		item->sprite->textures[0]->dim.width,
		item->sprite->textures[0]->dim.height
	};
	item->button = gui_button_create(area, event, NULL);
	if (linkedlist_size(m->items) == 0)
		item->button->hover = true;
	linkedlist_append(&m->items, item);
}

void
menu_render(Menu *m, Camera *cam)
{
	LinkedList *items = m->items;

	int index = 0;
	while (items) {
		MenuItem *item = items->data;
		items = items->next;
		if (m->selected == index)
			sprite_render(item->hsprite, cam);
		else
			sprite_render(item->sprite, cam);
		index++;
	}

}

static void
menu_item_destroy(MenuItem *item)
{
	if (item->sprite)
		sprite_destroy(item->sprite);
	if (item->hsprite)
		sprite_destroy(item->hsprite);
	gui_button_destroy(item->button);
	free(item);
}

void
menu_destroy(Menu *m)
{
	while (m->items)
		menu_item_destroy(linkedlist_pop(&m->items));

	free(m);
}

