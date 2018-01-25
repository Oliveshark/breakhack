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
	item->effect = NULL;
	return item;
}

void item_render(Item *item, Camera *cam)
{
	sprite_render(item->sprite, cam);
}

void
item_destroy(Item *item)
{
	if (item->sprite)
		sprite_destroy(item->sprite);
	free(item);
}
