#ifndef ITEM_H_
#define ITEM_H_

#include <stdbool.h>

#include "sprite.h"
#include "position.h"
#include "player.h"
#include "camera.h"

typedef struct Item_t {
	Sprite *sprite;
	bool collected;
	void (*effect)(struct Item_t *, Player *);
} Item;

Item *
item_create(void);

void
item_render(Item*, Camera*);

void
item_destroy(Item*);

#endif // ITEM_H_