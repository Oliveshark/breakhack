#include <stdbool.h>
#include "menu.h"
#include "util.h"
#include "sprite.h"
#include "defines.h"

typedef struct MenuItems_t {
	Sprite *sprite;
	Sprite *hsprite;
	bool selected;
	void (*onClick)(void);
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
	if (event->type != SDL_KEYDOWN && event->type != SDL_MOUSEMOTION) {
		return;
	}
	error("Menu events need implementation still");
	UNUSED(m);
}

void
menu_item_add(Menu *m, Sprite *s1, Sprite *s2)
{
	MenuItem *item = ec_malloc(sizeof(MenuItem));
	item->sprite = s1;
	item->hsprite = s2;
	item->selected = false;
	item->onClick = NULL;
	linkedlist_append(&m->items, item);
}

void
menu_render(Menu *m, Camera *cam)
{
	LinkedList *items = m->items;

	while (items) {
		MenuItem *item = items->data;
		sprite_render(item->sprite, cam);
		items = items->next;
	}
}

static void
menu_item_destroy(MenuItem *item)
{
	sprite_destroy(item->sprite);
	free(item);
}

void
menu_destroy(Menu *m)
{
	LinkedList *items = m->items;
	while (items) {
		menu_item_destroy(items->data);
		items = items->next;
	}
	free(m);
}

