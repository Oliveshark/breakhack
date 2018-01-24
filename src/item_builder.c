#include <SDL2/SDL.h>

#include "item_builder.h"
#include "texture.h"
#include "util.h"
#include "gui.h"

static ItemBuilder *builder = NULL;

void
item_builder_init(SDL_Renderer *renderer)
{
	builder = ec_malloc(sizeof(ItemBuilder));
	builder->textures = ht_create(20);
	builder->renderer = renderer;
}

static void
check_builder()
{
	if (!builder)
		fatal("item_builder_init() not run");
}

static Texture *
load_texture(const char *path)
{
	Texture *t = ht_get(builder->textures, path);
	if (!t) {
		t = texture_create();
		texture_load_from_file(t, path, builder->renderer);
		t->dim = (Dimension) { 32, 32 };
	}
	return t;
}

static void
add_health(Item *item, Player *player)
{
	int original_hp = player->stats.hp;
	player->stats.hp += 2;
	if (player->stats.hp > player->stats.maxhp)
		player->stats.hp = player->stats.maxhp;

	gui_log("You gained %d health", player->stats.hp - original_hp);
}

static Item *
create_health()
{
	Texture *t;
	Item *item;

	item = item_create();
	t = load_texture("assets/Items/Potion.png");

	item->sprite = sprite_create();
	sprite_set_texture(item->sprite, t, 0);
	item->sprite->clip = (SDL_Rect) { 0, 0, 16, 16 };
	item->effect = &add_health;

	return item;
}

Item *
item_builder_build_item(ItemKey key)
{
	check_builder();

	Item *item = NULL;
	switch (key) {
	case HEALTH:
		item = create_health(builder);
		break;
	default:
		fatal("in item_builder_build() : Unhandled item key %d", key);
		break;
	}

	return item;
}

void
item_builder_close()
{
	ht_destroy_custom(builder->textures, (void (*)(void*)) &texture_destroy);
	free(builder);
}