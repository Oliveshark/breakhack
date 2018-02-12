#include <stdlib.h>
#include <stdbool.h>
#include "menu.h"
#include "util.h"
#include "sprite.h"
#include "defines.h"
#include "gui_button.h"
#include "keyboard.h"

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

void
menu_handle_event(Menu *m, SDL_Event *event)
{
	LinkedList *items;
	bool reset_buttons = false;
	bool trigger_button = false;

	if (keyboard_direction_press(UP, event)) {
		m->selected--;
		reset_buttons = true;
	} else if (keyboard_direction_press(DOWN, event)) {
		m->selected++;
		reset_buttons = true;
	} else if (keyboard_press(SDLK_RETURN, event)) {
		trigger_button = true;
	}
	m->selected = m->selected % linkedlist_size(m->items);

	if (trigger_button) {
		MenuItem *item = linkedlist_get(&m->items, m->selected);
		if (item->button->event)
			item->button->event(item->button->usrdata);
		return;
	}

	int current_select = 0;
	bool mouse_selection = false;
	items = m->items;
	while (items) {
		MenuItem *item = items->data;
		items = items->next;
		if (reset_buttons)
			item->button->hover = false;

		gui_button_handle_event(item->button, event);
		if (item->button->hover) {
			m->selected = current_select;
			mouse_selection = true;
		}

		current_select++;
	}

	if (!mouse_selection)
		((MenuItem*) linkedlist_get(&m->items, m->selected))->button->hover = true;
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

