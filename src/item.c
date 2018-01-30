#include <stdlib.h>
#include "item.h"
#include "util.h"

Item *
item_create(void)
{
	Item *item = ec_malloc(sizeof(Item));
	item->sprite = NULL;
	item->collected = false;
	m_strcpy(item->label, 50, "");
	item->value = 0.0;
	item->items = NULL;
	item->effect = NULL;
	return item;
}

void
item_render(Item *item, Camera *cam)
{
	sprite_render(item->sprite, cam);
}

void
item_collected(Item *item, Player *player)
{
	LinkedList *items = item->items;

	while (items != NULL) {
		Item *subitem = items->data;
		items = items->next;
		item_collected(subitem, player);
	}

	if (item->effect != NULL)
		item->effect(item, player);

	item->collected = true;
}

void
item_destroy(Item *item)
{
	LinkedList *items;

	if (item->sprite)
		sprite_destroy(item->sprite);

	items = item->items;
	while (items != NULL) {
		Item *subitem = items->data;
		items->data = NULL;
		items = items->next;
		item_destroy(subitem);
	}
	linkedlist_destroy(&items);

	free(item);
}
