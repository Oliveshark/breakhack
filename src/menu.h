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

#ifndef MENU_H_
#define	MENU_H_

#include <SDL.h>
#include "camera.h"
#include "linkedlist.h"
#include "texture.h"
#include "sprite.h"

typedef struct TEXT_MENU_ITEM {
	char label[20];
	void (*callback)(void*);
} TEXT_MENU_ITEM;

typedef struct Menu_t {
	LinkedList *items;
	int selected;
} Menu;

Menu *
menu_create(void);

void
menu_create_text_menu(Menu **menu, TEXT_MENU_ITEM *menu_items, unsigned int size, SDL_Renderer *);

Menu *
menu_create_character_selector(void (*onCharacterSelect)(const char *));

void
menu_update(Menu*, Input*);

void
menu_item_add(Menu*, Sprite*, Sprite*, void (*)(void*));

void
menu_render(Menu*, Camera*);

void
menu_destroy(Menu*);

#endif // MENU_H_
