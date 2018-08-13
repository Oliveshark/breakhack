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

#include "object.h"
#include "util.h"
#include "mixer.h"
#include "random.h"
#include "texturecache.h"

Object *
object_create(void)
{
	Object *o = ec_malloc(sizeof(Object));
	o->sprite = sprite_create();
	o->blocking = false;
	o->damage = 0;
	o->timeout = -1;
	o->dead = false;
	return o;
}

Object *
object_create_fire()
{
	Object *o = object_create();
	Texture *t0 = texturecache_add("Objects/Effect0.png");
	Texture *t1 = texturecache_add("Objects/Effect1.png");
	sprite_set_texture(o->sprite, t0, 0);
	sprite_set_texture(o->sprite, t1, 1);
	o->sprite->dim = GAME_DIMENSION;
	o->sprite->clip = CLIP16(16, 21*16);
	o->damage = 3;
	o->timeout = 5;
	return o;
}

void
object_render(Object *o, Camera *cam)
{
	sprite_render(o->sprite, cam);
}

void
object_step(Object *o)
{
	if (o->timeout < 0)
		return;
	o->timeout--;
	if (o->timeout <= 0)
		o->dead = true;
}

void
object_damage(Object *o, Player *p)
{
	if (!o->damage)
		return;
	p->stats.hp -= o->damage;
	player_hit(p, o->damage);
}

void
object_destroy(Object *o)
{
	sprite_destroy(o->sprite);
	free(o);
}
