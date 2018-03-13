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
#include "projectile.h"
#include "util.h"
#include "texturecache.h"
#include "player.h"
#include "monster.h"
#include "mixer.h"
#include "gui.h"
#include "item_builder.h"
#include "random.h"

static void
onDaggerRender(Sprite *s)
{
	if (!timer_started(s->renderTimer))
		timer_start(s->renderTimer);

	if (timer_get_ticks(s->renderTimer) > 60) {
		timer_start(s->renderTimer);
		s->clip.x += 16;
		s->clip.x = s->clip.x % 64;
	}
}

Projectile *
projectile_dagger_create(void)
{
	Projectile *p = projectile_create();
	sprite_set_texture(p->sprite, texturecache_add("Extras/Dagger.png"), 0);
	p->sprite->onRender = onDaggerRender;
	p->sprite->animate = false;
	p->sprite->clip = CLIP16(0, 0);
	p->sprite->dim = (Dimension) { 32, 32 };
	return p;
}

Projectile *
projectile_create(void)
{
	Projectile *p = ec_malloc(sizeof(Projectile));
	p->sprite = sprite_create();
	p->velocity = VECTOR2D_NODIR;
	p->alive = true;
	p->lifetime = timer_create();
	p->onRender = NULL;
	timer_start(p->lifetime);
	return p;
}

void
projectile_update(Projectile *p, Player *player, RoomMatrix *rm, float deltatime)
{
	p->sprite->pos.x += (int) (p->velocity.x * deltatime);
	p->sprite->pos.y += (int) (p->velocity.y * deltatime);

	if (timer_get_ticks(p->lifetime) > 2000)
		p->alive = false;

	Position collisionPos = p->sprite->pos;
	if (p->velocity.x > 0)
		collisionPos.x += TILE_DIMENSION;
	else if(p->velocity.y > 0)
		collisionPos.y += TILE_DIMENSION;

	Position roomPos = position_to_matrix_coords(&collisionPos);
	RoomSpace *space = &rm->spaces[roomPos.x][roomPos.y];
	if (!space->occupied)
		return;

	if (space->player)
		return;

	if (space->monster) {
		Uint32 dmg = stats_fight(&player->stats, &space->monster->stats);
		if (dmg > 0) {
			gui_log("Your dagger pierced %s for %u damage", space->monster->lclabel, dmg);
			mixer_play_effect(SWORD_HIT);
			player->hits += 1;
		}
		/*
		 * TODO(Linus): This can be fixed so that daggers
		 * can be retrieved. Probably best to create an "UpdateData" container that
		 * can be sent as arguments down the update queue.

		 if (get_random(1) == 0) {
			debug("Adding dagger item");
			Item *item = item_builder_build_item(DAGGER, 1);
			item->sprite->pos = space->monster->sprite->pos;
			linkedlist_append(&map->items, item);
		}
		*/
		monster_hit(space->monster, dmg);
	}
	p->alive = false;
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
