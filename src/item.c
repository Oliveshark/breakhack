#include "item.h"
#include "util.h"

Item *
item_create(void)
{
	Item *item = ec_malloc(sizeof(Item));
	item->sprite = NULL;
	item->collected = false;
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