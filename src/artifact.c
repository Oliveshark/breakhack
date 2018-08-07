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

#include "artifact.h"
#include "util.h"
#include "texturecache.h"

static void
artifact_set_effect(Artifact *a, MagicalEffect effect)
{
	a->effect = effect;
	switch (effect) {
		case IMPROVED_HEARING:
			a->info.name = "Potion of ear juice";
			a->info.desc = "Your hearing is slightly improved";
			break;
		case TRAP_AVOIDANCE:
			a->info.name = "Boot with nails inside";
			a->info.desc = "You are lighter on your feet";
			break;
		case PIERCING_DAGGERS:
			a->info.name = "Whetstone";
			a->info.desc = "Your daggers are sharper";
			Texture *t = texturecache_add("Items/Rock.png");
			sprite_set_texture(a->sprite, t, 0);
			a->sprite->clip = CLIP16(0, 0);
			break;
		case CHARGE_THROUGH:
			a->info.name = "Greasy shield";
			a->info.desc = "You glide through obstructions";
			break;
		case PUSH_BACK:
			a->info.name = "Glove of strength";
			a->info.desc = "Your arm is stronger";
			break;
		case DAGGER_RECOVERY:
			a->info.name = "Forging hammer";
			a->info.desc = "Your daggers are more durable";
			break;
		case INCREASED_STUN:
			a->info.name = "Solid shield";
			a->info.desc = "Your shield is harder";
			break;
		case FEAR_INDUCING:
			a->info.name = "Ugly shirt";
			a->info.desc = "You look disgusting";
			break;
		default:
			break;
	}
}

Artifact *
artifact_create(MagicalEffect effect)
{
	Artifact *a = ec_malloc(sizeof(Artifact));
	a->sprite = sprite_create();
	a->sprite->dim = GAME_DIMENSION;
	a->collected = false;
	a->level = 1;
	artifact_set_effect(a, effect);
	return a;
}

Artifact *
artifact_copy(Artifact *a)
{
	Artifact *new = ec_malloc(sizeof(Artifact));
	*new = *a;
	return new;
}

void
artifact_render(Artifact *a, Camera *cam)
{
	sprite_render(a->sprite, cam);
}

void
artifact_destroy(Artifact *a)
{
	sprite_destroy(a->sprite);
	free(a);
}
