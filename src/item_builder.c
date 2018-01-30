#include <SDL2/SDL.h>
#include <stdlib.h>

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
check_builder(void)
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
eat_flesh(Item *item, Player *player)
{
	int original_hp = player->stats.hp;
	player->stats.hp += (int) item->value;
	if (player->stats.hp > player->stats.maxhp)
		player->stats.hp = player->stats.maxhp;

	gui_log("You eat some foul meat and gain %d health", player->stats.hp - original_hp);
}

static void
drink_health(Item *item, Player *player)
{
	int original_hp = player->stats.hp;
	player->stats.hp += (int) item->value;
	if (player->stats.hp > player->stats.maxhp)
		player->stats.hp = player->stats.maxhp;

	gui_log("You drink a health potion and gain %d health", player->stats.hp - original_hp);
}

static Item *
create_item(const char *path, SDL_Rect clip, void (*cb)(Item*, Player*))
{
	Texture *t;
	Item *item;

	item = item_create();
	t = load_texture(path);

	item->sprite = sprite_create();
	sprite_set_texture(item->sprite, t, 0);
	item->sprite->clip = clip;
	item->effect = cb;

	return item;
}

static void
pickup_gold(Item *item, Player *player)
{
	player->gold += item->value;
	gui_log("You pick up %s", &item->label);
}

static Item *
create_treasure(void)
{
	unsigned int value = rand() % TREASURE_COUNT;
	double amt = (unsigned int) rand() % 40;
	char label[50];

	SDL_Rect clip = { 0, 0, 16, 16 };
	switch (value) {
		case COPPER:
			m_sprintf(&label[0], 50, "%.0f copper", amt);
			amt /= 100;
			break;
		case SILVER:
			m_sprintf(&label[0], 50, "%.0f silver", amt);
			clip.x = 48;
			amt /= 10;
			break;
		case GOLD:
			m_sprintf(&label[0], 50, "%.0f gold", amt);
			clip.y = 16;
			break;
		case PLATINUM:
			m_sprintf(&label[0], 50, "%.0f platinum", amt);
			clip.x = 48;
			clip.y = 16;
			amt *= 10;
			break;
		default:
			break;
	}

	if (amt > 15 && amt < 30)
		clip.x += 16;
	else if (amt <= 15)
		clip.x += 32;

	Item *item = create_item("assets/Items/Money.png", clip, &pickup_gold);
	m_strcpy(item->label, 50, label);
	item->value = amt;
	return item;
}

Item *
item_builder_build_item(ItemKey key)
{
	static const char *path_flesh	= "assets/Items/Flesh.png";
	static const char *path_potion	= "assets/Items/Potion.png";

	check_builder();

	Item *item = NULL;
	switch (key) {
		case TREASURE:
			item = create_treasure();
			break;
		case FLESH:
			item = create_item(path_flesh,
					   (SDL_Rect) { 0, 0, 16, 16 },
					   &eat_flesh);
			item->value = 1;
			break;
		case HEALTH:
			item = create_item(path_potion,
					   (SDL_Rect) { 0, 0, 16, 16 },
					   &drink_health);
			item->value = 1 + (rand() % 2);
			break;
		default:
			fatal("in item_builder_build() : Unhandled item key %d", key);
			break;
	}

	return item;
}

Item *
item_builder_build_sack(void)
{
	return create_item("assets/Items/Chest0.png",
			   (SDL_Rect) { 0, 32, 16, 16 },
			   NULL);
}

void
item_builder_close(void)
{
	ht_destroy_custom(builder->textures, (void (*)(void*)) &texture_destroy);
	free(builder);
}
