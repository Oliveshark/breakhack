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

#include <stdlib.h>
#include <string.h>
#include "projectile.h"
#include "util.h"
#include "texturecache.h"
#include "player.h"
#include "monster.h"
#include "mixer.h"
#include "gui.h"
#include "item_builder.h"
#include "random.h"
#include "update_data.h"
#include "effect_util.h"
#include "particle_engine.h"

static void
onDaggerRender(Sprite *s)
{
	if (s->angle >= 0)
		s->angle += 30;
	else
		s->angle -= 30;
}

Projectile *
projectile_dagger_create(void)
{
	Projectile *p = projectile_create();
	sprite_set_texture(p->sprite, texturecache_add("Items/ShortWep.png"), 0);
	p->sprite->onRender = onDaggerRender;
	p->sprite->animate = false;
	p->sprite->clip = CLIP16(0, 0);
	p->sprite->dim = (Dimension) { 32, 32 };
	p->sprite->rotationPoint = (SDL_Point) { 16, 16 };
	p->collisionCount = 0;
	p->bounceCount = 0;
	memset(&p->processedSpaces,
	       false,
	       sizeof(p->processedSpaces[0][0]) * MAP_ROOM_WIDTH * MAP_ROOM_HEIGHT);
	return p;
}

Projectile *
projectile_create(void)
{
	Projectile *p = ec_malloc(sizeof(Projectile));
	p->sprite = sprite_create();
	p->velocity = VECTOR2D_NODIR;
	p->alive = true;
	p->lifetime = _timer_create();
	p->onRender = NULL;
	timer_start(p->lifetime);
	return p;
}

static Position
get_collision_pos_for(Projectile *p)
{
	Position collisionPos = p->sprite->pos;
	if (p->velocity.x > 0) collisionPos.x += TILE_DIMENSION;
	if(p->velocity.y > 0) collisionPos.y += TILE_DIMENSION;
	return collisionPos;
}

static Position
get_projectile_pos_for(Projectile *p)
{
	Position projectilePos = p->sprite->pos;
	if (p->velocity.x < 0) projectilePos.x += TILE_DIMENSION;
	if (p->velocity.y < 0) projectilePos.y += TILE_DIMENSION;
	return projectilePos;
}

static void
clear_processed_spaces(Projectile *p)
{
	memset(&p->processedSpaces,
	       false,
	       sizeof(p->processedSpaces[0][0]) * MAP_ROOM_WIDTH * MAP_ROOM_HEIGHT);
}

static void
perform_dagger_explosion(Player *player, RoomMatrix *rm, Position *collisionPos)
{
	if (player_has_artifact(player, VOLATILE_DAGGERS)) {
		mixer_play_effect(EXPLOSION_EFFECT);
		particle_engine_fire_explosion(*collisionPos, DIM(32, 32));
		effect_damage_surroundings(collisionPos,
					   rm,
					   player,
					   &player->stats,
					   player_has_artifact(player, VOLATILE_DAGGERS),
					   0,
					   false);
	}
}

void
projectile_update(Projectile *p, UpdateData *data)
{
	bool alive = false;

	p->sprite->pos.x += (int) (p->velocity.x * data->deltatime);
	p->sprite->pos.y += (int) (p->velocity.y * data->deltatime);

	if (timer_get_ticks(p->lifetime) > 2000)
		p->alive = false;

	Position collisionPos = get_collision_pos_for(p);
	if (!position_in_room(&collisionPos, &data->map->currentRoom)) {
		p->alive = false;
		return;
	}

	Position roomPos = position_to_matrix_coords(&collisionPos);
	if (p->processedSpaces[roomPos.x][roomPos.y])
		return;
	p->processedSpaces[roomPos.x][roomPos.y] = true;

	RoomSpace *space = &data->matrix->spaces[roomPos.x][roomPos.y];
	if (!space->occupied && !space->monster)
		return;

	if (space->player)
		return;

	Position projectilePos = get_projectile_pos_for(p);
	if (space->monster) {
		Stats tmpStats = data->player->stats;
		tmpStats.dmg *= 2;
		CombatResult result = stats_fight(&tmpStats, &space->monster->stats);
		if (result.dmg > 0) {
			gui_log("Your dagger pierced %s for %u damage", space->monster->lclabel, result.dmg);
			data->player->stat_data.hits += 1;
			perform_dagger_explosion(data->player, data->matrix, &collisionPos);
		} else {
			gui_log("%s dodged your dagger", space->monster->label);
		}
		monster_hit(space->monster, result.dmg, result.critical);
		player_monster_kill_check(data->player, space->monster);
		alive = player_has_artifact(data->player, PIERCING_DAGGERS) > p->collisionCount;
		projectilePos = space->monster->sprite->pos;
	} else {
		p->bounceCount += 1;
		vector2d_reverse(&p->velocity);
		clear_processed_spaces(p);
		alive = p->bounceCount <= player_has_artifact(data->player, DAGGER_BOUNCE);
	}

	mixer_play_effect(SWORD_HIT);
	if (!alive && get_random(5) <= player_has_artifact(data->player, DAGGER_RECOVERY)) {
		Item *item = item_builder_build_item(DAGGER, 1);
		item->sprite->pos = position_to_tile_pos(&projectilePos);
		linkedlist_append(&data->map->items, item);
	}
	p->alive = alive;
	p->collisionCount++;
}

void
projectile_render(Projectile *p, Camera *cam)
{
	if (p->onRender)
		p->onRender(p);
	sprite_render(p->sprite, cam);
}

void
projectile_destroy(Projectile *p)
{
	sprite_destroy(p->sprite);
	timer_destroy(p->lifetime);
	free(p);
}
