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
#include <string.h>
#include "menu.h"
#include "util.h"
#include "sprite.h"
#include "defines.h"
#include "gui_button.h"
#include "keyboard.h"
#include "mixer.h"
#include "collisions.h"
#include "texturecache.h"

static SDL_Color C_MENU_DEFAULT		= { 255, 255, 0, 255 };
static SDL_Color C_MENU_OUTLINE_DEFAULT	= { 0,	0, 0, 255 };
static SDL_Color C_MENU_HOVER		= { 255, 0, 0, 255 };

typedef struct MenuItem {
	Sprite *sprite;
	Sprite *hsprite;
	GuiButton *button;
} MenuItem;

static void redraw_description(Menu *m, SDL_Renderer *renderer);

Menu *
menu_create(void)
{
	Menu *menu = ec_malloc(sizeof(Menu));
	menu->items = linkedlist_create();
	menu->descriptions = linkedlist_create();
	menu->selected = 0;
	menu->menuDescription = sprite_create();
	sprite_load_text_texture(menu->menuDescription,
				 "GUI/SDS_8x8.ttf",
				 0,
				 10,
				 1);
	menu->menuDescription->fixed = true;
	menu->menuDescription->pos = POS(20, SCREEN_HEIGHT - 20);
	menu->menuDescription->hidden = true;

	return menu;
}

void
menu_create_text_menu(Menu **menu, TEXT_MENU_ITEM *menu_items, unsigned int size, SDL_Renderer *renderer)
{
	if (*menu != NULL) {
		menu_destroy(*menu);
		*menu = NULL;
	}

	*menu = menu_create();

	for (unsigned int i = 0; i < size; ++i) {
		unsigned int hcenter;

		Sprite *s1 = sprite_create();
		sprite_load_text_texture(s1, "GUI/SDS_8x8.ttf", 0, 25, 2);
		texture_load_from_text(s1->textures[0], menu_items[i].label,
				       C_MENU_DEFAULT, C_MENU_OUTLINE_DEFAULT, renderer);

		hcenter = (SCREEN_WIDTH/2) - (s1->textures[0]->dim.width/2);
		s1->pos = (Position) { (int) hcenter, (int) 200 + (i*50) };
		s1->dim = s1->textures[0]->dim;
		s1->fixed = true;

		Sprite *s2 = sprite_create();
		sprite_load_text_texture(s2, "GUI/SDS_8x8.ttf", 0, 25, 2);
		texture_load_from_text(s2->textures[0], menu_items[i].label,
				       C_MENU_HOVER, C_MENU_OUTLINE_DEFAULT, renderer);

		s2->pos = (Position) { (int) hcenter, (int) 200 + (i*50) };
		s2->dim = s2->textures[0]->dim;
		s2->fixed = true;

		menu_item_add(*menu, s1, s2, menu_items[i].callback);
		linkedlist_append(&(*menu)->descriptions, (void*) menu_items[i].description);
	}

	(*menu)->selected = 0;
	redraw_description(*menu, renderer);
}

Menu *
menu_create_character_selector(void (*onCharacterSelect)(const char *), Camera *cam)
{
	static const char *spriteSheets[] = {
		"Commissions/Warrior.png",
		"Commissions/Rogue.png",
		"Commissions/Mage.png"
	};

	static const char *callbackData[] = {
		"warrior",
		"rogue",
		"mage"
	};

	static const char *descriptions[] = {
		"Play as the warrior",
		"Play as the rogue",
		"Play as the mage"
	};

	Menu *menu = menu_create();
	int xoffset = 168;
	for (size_t i = 0; i < 3; ++i) {
		Sprite *s1 = sprite_create();
		sprite_set_texture(s1, texturecache_add(spriteSheets[i]), 0);
		s1->clip = CLIP16(0, 48);
		s1->dim = DIM(64, 64);
		s1->pos = POS(xoffset - 32, 256);
		s1->fixed = true;

		Sprite *s2 = sprite_create();
		sprite_set_texture(s2, texturecache_add(spriteSheets[i]), 0);
		s2->clip = CLIP16(0, 0);
		s2->dim = DIM(64, 64);
		s2->pos = POS(xoffset - 32, 256);
		s2->fixed = true;

		menu_item_add(menu, s1, s2, (void (*)(void *)) onCharacterSelect);
		MenuItem *item = linkedlist_get(&menu->items, (Uint32) i);
		item->button->usrdata = callbackData[i];
		xoffset += 168;

		linkedlist_append(&menu->descriptions, descriptions[i]);
	}

	menu->selected = 0;
	redraw_description(menu, cam->renderer);

	return menu;
}

static bool
handle_keyboard_input(Menu *m, Input *input)
{
	int lastSelect = -1;

	if (input_key_is_pressed(input, KEY_UP | KEY_LEFT)) {
		lastSelect = m->selected;
		m->selected--;
	} else if (input_key_is_pressed(input, KEY_DOWN | KEY_RIGHT)) {
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

	int lastSelect = m->selected;
	int index = 0;
	LinkedList *items = m->items;
	while (items) {
		MenuItem *item = items->data;
		items = items->next;

		if (position_in_rect(&p, &item->button->area)) {
			m->selected = index;
			if (index != lastSelect)
				mixer_play_effect(CLICK);
			break;
		}
		index++;
	}

}

static void
redraw_description(Menu *m, SDL_Renderer *renderer)
{
	char *description = linkedlist_get(&m->descriptions, m->selected);
	if (!description || strlen(description) <= 1) {
		m->menuDescription->hidden = true;
		return;
	}

	m->menuDescription->hidden = false;
	texture_load_from_text(m->menuDescription->textures[0],
			       description,
			       C_WHITE,
			       C_BLACK,
			       renderer);
	m->menuDescription->dim = DIM(
		m->menuDescription->textures[0]->dim.width,
		m->menuDescription->textures[0]->dim.height);
}

void
menu_update(Menu *m, Input *input, Camera *cam)
{
	if (!m)
		return;

	static int lastSelected = -1;

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
			item->button->event(item->button->usrdata);
			return;
		}
	}

	if (lastSelected != m->selected) {
		lastSelected = m->selected;
		redraw_description(m, cam->renderer);
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
	if (!m)
		return;

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

	sprite_render(m->menuDescription, cam);

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

	while (m->descriptions)
		linkedlist_pop(&m->descriptions);

	sprite_destroy(m->menuDescription);
	free(m);
}

