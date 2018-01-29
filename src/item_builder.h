#ifndef ITEMBUILDER_H_
#define ITEMBUILDER_H_

#include "item.h"
#include "hashtable.h"

typedef struct {
	Hashtable *textures;
	SDL_Renderer *renderer;
} ItemBuilder;

typedef enum {
	COPPER,
	SILVER,
	GOLD,
	PLATINUM,
	TREASURE_COUNT
} Treasure;

typedef enum {
	FLESH,
	HEALTH,
	TREASURE,
	ITEM_COUNT
} ItemKey;

void
item_builder_init(SDL_Renderer *);

Item *
item_builder_build_item(ItemKey key);

void
item_builder_close(void);

#endif // ITEMBUILDER_H_
