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
#include "loot.h"
#include "map.h"
#include "item.h"
#include "item_builder.h"
#include "random.h"
#include "gui.h"
#include "mixer.h"

#define MAX_ITEM_DROP 16
#define ITEM_DROP_TIME_MS 175
#define DROP_POSITION_COUNT 9

static bool item_dropped = false;

typedef enum {
	DROP_NONE,
	DROP_WEAPON,
	DROP_COINS,
	DROP_POTION,
	DROP_FLESH,
	DROP_ARTIFACT,
	DROP_KEY,
	DROP_HEALTH
} DropType;

static void
on_weapon_drop(void)
{
	mixer_play_effect(METAL_DROP);
}

static void
on_coin_drop(void)
{
	if (get_random(1) == 1) {
		mixer_play_effect(COIN_DROP1);
	} else {
		mixer_play_effect(COIN_DROP2);
	}
}

static void
on_potion_drop(void)
{
	mixer_play_effect(POTION_DROP);
}

static void
on_flesh_drop(void)
{
	if (get_random(1) == 1) {
		mixer_play_effect(FLESH_DROP1);
	} else {
		mixer_play_effect(FLESH_DROP2);
	}
}

static void
on_health_drop(void)
{
	mixer_play_effect(BOTTLE);
}

static void *
get_drop_callback(DropType type)
{
	switch (type) {
		case DROP_WEAPON:
			return on_weapon_drop;
		case DROP_COINS:
			return on_coin_drop;
		case DROP_POTION:
			return on_potion_drop;
		case DROP_FLESH:
			return on_flesh_drop;
		case DROP_HEALTH:
			return on_health_drop;
		case DROP_NONE:
		default:
			return NULL;
	}
}


static inline void drop_item(Sprite *sprite, Position *drop_positions,
			     size_t num_drop_positions, DropType type)
{
	/* Avoid FPE (division by zero) */
	Position drop_pos;
	if (num_drop_positions == 0) {
		drop_pos = sprite->pos;
	} else {
		drop_pos = drop_positions[get_random((uint32_t) num_drop_positions - 1)];
	}

	/* Create an interpolation dest */
	Destination dest = {
		.pos = drop_pos,
		.angle = 0,
		.dim = DIM(32, 32),
		.time_ms = (float)(ITEM_DROP_TIME_MS + get_random(50)),
		.on_complete = get_drop_callback(type),
	};

	/* Make the item smaller to simulate it flying */
	sprite->dim = DIM(4, 4);
	sprite->angle = (float) get_random(720) + 360;

	sprite_interpolate_to(sprite, &dest);

	item_dropped = true;
}

void loot_drop(Monster *monster, Map *map, RoomMatrix *rm, Player *player)
{
	static Uint32 treasure_drop_chance = 1;
	static Uint32 item_drop_chance = 1;
	static Uint32 potion_drop_chance = 30;

	if (monster->sprite->state == SPRITE_STATE_FALLING)
		return;

	/* Reset drop bool */
	item_dropped = false;

	Item *item;
	Position drop_positions[DROP_POSITION_COUNT];
	bool player_full_health = player->stats.hp >= player->stats.maxhp;

	Position monsterTilePos = position_to_tile_pos(&monster->sprite->pos);

	/* Find possible loot drop positions */
	size_t space_count = roommatrix_get_surrounding_spaces(rm, &monsterTilePos, drop_positions, DROP_POSITION_COUNT);

	/* Drop artifacts */
	if (monster->boss) {
		Artifact *a = artifact_create_random(player, 2);
		a->sprite->pos = monsterTilePos;
		linkedlist_append(&map->artifacts, a);
		drop_item(a->sprite, drop_positions, space_count, DROP_ARTIFACT);
	}

	if (monster->stats.lvl > 2 && get_random(29) == 0) {
		Artifact *a = artifact_create_random(player, 1);
		a->sprite->pos = monsterTilePos;
		linkedlist_append(&map->artifacts, a);
		drop_item(a->sprite, drop_positions, space_count, DROP_ARTIFACT);
	}

	/* Drop random loot */
	if (monster->boss) {
		Item *treasure = item_builder_build_item(TREASURE, map->level*2);
		treasure->sprite->pos = monsterTilePos;
		linkedlist_append(&map->items, treasure);
		drop_item(treasure->sprite, drop_positions, space_count, DROP_COINS);
	}

	if (monster->items.keyType != LOCK_NONE) {
		Item *key = item_builder_build_key(monster->items.keyType);
		key->sprite->pos = monsterTilePos;
		linkedlist_append(&map->items, key);
		drop_item(key->sprite, drop_positions, space_count, DROP_KEY);
	}

	if (strcmp(monster->label, "The Trader") == 0) {
		Item *treasure = item_builder_build_treasure(PLATINUM, 10 * monster->stats.lvl);
		treasure->sprite->pos = monsterTilePos;
		linkedlist_append(&map->items, treasure);
		drop_item(treasure->sprite, drop_positions, space_count, DROP_COINS);
	}

	if (strcmp(monster->label, "A Fairy") == 0) {
		Item *treasure = item_builder_build_treasure(PLATINUM, 3 * monster->stats.lvl);
		treasure->sprite->pos = monsterTilePos;
		linkedlist_append(&map->items, treasure);
		drop_item(treasure->sprite, drop_positions, space_count, DROP_COINS);
	}

	if (player->stats.hp < player->stats.maxhp / 2)
		item_drop_chance = 0;

	if (player_has_potion_effect(player, POTION_NONE) && get_random(potion_drop_chance) == 0) {
		if (get_random(1) == 0)
			item = item_builder_build_potion(POTION_BLOODLUST);
		else
			item = item_builder_build_potion(POTION_FROST);
		item->sprite->pos = monsterTilePos;
		linkedlist_append(&map->items, item);
		drop_item(item->sprite, drop_positions, space_count, DROP_POTION);
	}
	if (get_random(treasure_drop_chance) == 0) {
		item = item_builder_build_item(TREASURE, map->level);
		item->sprite->pos = monsterTilePos;
		linkedlist_append(&map->items, item);
		drop_item(item->sprite, drop_positions, space_count, DROP_COINS);
	}
	if (get_random(item_drop_chance) == 0) {
		ItemKey key;
		key = get_random(DAGGER);
		item = item_builder_build_item(key, map->level);
		item->sprite->pos = monsterTilePos;
		linkedlist_append(&map->items, item);
		drop_item(item->sprite, drop_positions,
			  space_count,
			  key == DAGGER ? DROP_WEAPON : DROP_HEALTH);
	}
	if (!player_full_health && get_random(2) == 0) {
		item = item_builder_build_item(FLESH, map->level);
		item->sprite->pos = monsterTilePos;
		linkedlist_append(&map->items, item);
		drop_item(item->sprite, drop_positions, space_count, DROP_FLESH);
	}

	if (item_dropped) {
		gui_log("%s dropped something", monster->label);
	}
}
