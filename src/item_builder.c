/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <SDL3/SDL.h>
#include <stdlib.h>
#include <assert.h>

#include "item_builder.h"
#include "texture.h"
#include "util.h"
#include "gui.h"
#include "mixer.h"
#include "random.h"
#include "texturecache.h"
#include "sprite.h"
#include "sprite_util.h"
#include "map.h"

static ItemBuilder *builder = NULL;

void
item_builder_init(SDL_Renderer *renderer)
{
	builder = ec_malloc(sizeof(ItemBuilder));
	builder->renderer = renderer;
}

static void
check_builder(void)
{
	if (!builder)
		fatal("item_builder_init() not run");
}

static void
eat_flesh(Item *item, Player *player)
{
	int original_hp = player->stats.hp;
	player->stats.hp += (int) item->value;
	if (player->stats.hp > player->stats.maxhp)
		player->stats.hp = player->stats.maxhp;

	mixer_play_effect(EAT);
	gui_log("You eat some foul meat and gain %d health",
		player->stats.hp - original_hp);
}

static void
drink_health(Item *item, Player *player)
{
	player->potion_sips += (int) item->value;

	mixer_play_effect(BOTTLE);
	gui_log("You collect %u sips of health", (unsigned int) item->value);
}

static void
pickup_dagger(Item *item, Player *player)
{
	player->daggers += (Uint32) item->value;

	mixer_play_effect(DAGGER_PICKUP);
	double count = player->class == ROGUE ? item->value * 2 : item->value;
	if (count > 1)
		gui_log("You collect %u daggers", (Uint32) count);
	else
		gui_log("You collect a dagger");
}

static Item *
create_item(const char *path0, const char *path1, SDL_Rect clip, void (*cb)(Item*, Player*))
{
	Item *item;

	item = item_create();
	Texture *t0 = texturecache_add(path0);
	Texture *t1 = NULL;
	if (path1)
		t1 = texturecache_add(path1);

	item->sprite = sprite_create();
	sprite_set_texture(item->sprite, t0, 0);
	sprite_set_texture(item->sprite, t1, 1);
	item->sprite->dim = GAME_DIMENSION;
	item->sprite->clip = clip;
	item->effect = cb;

	return item;
}

static void
pickup_gold(Item *item, Player *player)
{
	player->gold += item->value;
	mixer_play_effect(COIN);
	gui_log("You pick up %s", &item->label);
}

Item *
item_builder_build_treasure(Treasure type, double goldAmt)
{
	double amt = goldAmt;
	char label[50] = "";
	SDL_Rect clip = CLIP16(0, 0);
	switch (type) {
		case COPPER:
			m_sprintf(label, 50, "%.0f copper", amt);
			amt /= 100;
			break;
		case SILVER:
			m_sprintf(label, 50, "%.0f silver", amt);
			clip.x = 48;
			amt /= 10;
			break;
		case GOLD:
			m_sprintf(label, 50, "%.0f gold", amt);
			clip.y = 16;
			break;
		case PLATINUM:
			m_sprintf(label, 50, "%.0f platinum", amt);
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

	Item *item = create_item("Items/Money.png", NULL, clip, &pickup_gold);
	m_strcpy(item->label, 50, label);
	item->value = amt;
	return item;
}

static void
pickup_silver_key(Item *item, Player *player)
{
	gui_log("You pickup %s", item->label);
	mixer_play_effect(KEY_PICKUP);
	player->equipment.keys |= LOCK_SILVER;
}

static void
pickup_gold_key(Item *item, Player *player)
{
	gui_log("You pickup %s", item->label);
	mixer_play_effect(KEY_PICKUP);
	player->equipment.keys |= LOCK_GOLD;
}

Item *
item_builder_build_key(unsigned int type)
{
	char label[20];
	Item *item;
	switch (type) {
		case 1:
			m_sprintf(label, 20, "a silver key");
			item = create_item("Extras/Keys.png",
					   NULL,
					   CLIP16(0, 0),
					   &pickup_silver_key);
			break;
		case 2:
			m_sprintf(label, 20, "a gold key");
			item = create_item("Extras/Keys.png",
					   NULL,
					   CLIP16(16, 0),
					   &pickup_gold_key);
			break;
		default:
			fatal("Bad keytype provided");
	}

	m_strcpy(item->label, 20, label);
	return item;
}

static Item *
create_treasure(int current_level)
{
	double amt;
	unsigned int highest_treasure;
	unsigned int value;

	amt = (unsigned int) 1 + get_random(5*current_level) % 40;

	if (current_level > 9) {
		highest_treasure = PLATINUM;
	} else if (current_level > 3) {
		highest_treasure = GOLD;
	} else {
		highest_treasure = SILVER;
	}

	value = get_random(highest_treasure);
	return item_builder_build_treasure((Treasure) value, amt);
}

Item *
item_builder_build_item(ItemKey key, int level)
{
	static const char *path_flesh		= "Items/Flesh.png";
	static const char *path_potion		= "Items/Potion.png";
	static const char *path_short_wep	= "Items/ShortWep.png";

	check_builder();

	Item *item = NULL;
	switch (key) {
		case TREASURE:
			item = create_treasure(level*2);
			break;
		case FLESH:
			item = create_item(path_flesh,
					   NULL,
					   CLIP16(get_random(7) * 16, get_random(1) * 16),
					   &eat_flesh);
			item->value = 1 + get_random(level);
			break;
		case HEALTH:
			item = create_item(path_potion,
					   NULL,
					   CLIP16(0, 0),
					   &drink_health);
			item->value = 1 + get_random(level);
			break;
		case DAGGER:
			item = create_item(path_short_wep,
					   NULL,
					   CLIP16(0, 0),
					   &pickup_dagger);
			item->value = 1;
			break;
		default:
			fatal("in item_builder_build() : Unhandled item key %d", key);
			break;
	}

	if (item->value != 1) {
		Sprite *valueSprite = sprite_util_create_text_sprite("GUI/SDS_8x8.ttf",
								     8,
								     C_BLUE,
								     C_BLACK,
								     "%g",
								     item->value);
		valueSprite->offset.x = item->sprite->dim.width - valueSprite->dim.width;
		valueSprite->offset.y = item->sprite->dim.height - valueSprite->dim.height;
		linkedlist_append(&item->subsprites, valueSprite);
	}

	return item;
}

Item *
item_builder_build_sack(void)
{
	return create_item("Items/Chest0.png",
			   NULL,
			   CLIP16(0, 32),
			   NULL);
}

Item *
item_builder_build_container(const char *path0, const char *path1, SDL_Rect clip)
{
	Item *chest = create_item(path0,
				  path1,
				  clip,
				  NULL);
	chest->openable = true;
	chest->sprite->animate = false;
	return chest;
}

void
item_builder_close(void)
{
	free(builder);
}
