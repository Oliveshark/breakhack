#include <stdlib.h>
#include <stdbool.h>
#include "menu.h"
#include "util.h"
#include "sprite.h"
#include "defines.h"
#include "gui_button.h"
#include "keyboard.h"
#include "mixer.h"

typedef struct MenuItems_t {
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

static void
handle_keyboard_event(Menu *m, SDL_Event *event)
{
	int lastSelect = -1;

	if (keyboard_direction_press(UP, event)) {
		lastSelect = m->selected;
		m->selected--;
	} else if (keyboard_direction_press(DOWN, event)) {
		lastSelect = m->selected;
		m->selected++;
	} else if (keyboard_press(SDLK_RETURN, event)) {
		MenuItem *item = linkedlist_get(&m->items, m->selected);
		if (item->button->event)
			item->button->event(item->button->usrdata);
		return;
	} else {
		return;
	}
	m->selected = m->selected % linkedlist_size(m->items);

	if (lastSelect != -1)
		mixer_play_effect(CLICK);

	((MenuItem*) linkedlist_get(&m->items, lastSelect))->button->hover = false;
	((MenuItem*) linkedlist_get(&m->items, m->selected))->button->hover = true;
}

static void
handle_mouse_motion_event(Menu *m, SDL_Event *event)
{
	LinkedList *items;
	MenuItem *item;
	int current_select;
	bool activeItemFound = false;

	items = m->items;
	current_select = 0;
	while (items) {
		item = items->data;
		items = items->next;

		item->button->hover = false;
		gui_button_handle_event(item->button, event);
		if (item->button->hover) {
			if (current_select != m->selected) {
				mixer_play_effect(CLICK);
				m->selected = current_select;
			}
			activeItemFound = true;
		}
		current_select++;
	}

	if (!activeItemFound)
		((MenuItem*) linkedlist_get(&m->items, m->selected))->button->hover = true;
}

static void
handle_mouse_button_event(Menu *m, SDL_Event *event)
{
	/* NOTE: In some cases the button/item is destroyed by the click action
	 * make sure you don't 'use' items after a click event has fired. It
	 * might break. */

	MenuItem *item = linkedlist_get(&m->items, m->selected);
	gui_button_handle_event(item->button, event);
}

void
menu_handle_event(Menu *m, SDL_Event *event)
{
	if (event->type == SDL_KEYDOWN)
		handle_keyboard_event(m, event);
	else if (event->type == SDL_MOUSEMOTION)
		handle_mouse_motion_event(m, event);
	else if (event->type == SDL_MOUSEBUTTONDOWN)
		handle_mouse_button_event(m, event);
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

	while (items) {
		MenuItem *item = items->data;
		items = items->next;
		if (item->button->hover)
			sprite_render(item->hsprite, cam);
		else
			sprite_render(item->sprite, cam);
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

