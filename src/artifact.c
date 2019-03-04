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
#include "particle_engine.h"
#include "player.h"
#include "random.h"

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
		case PHASE_IMPROVEMENT:
			a->info.name = "Shadow cloak";
			a->info.desc = "You feel more in phase with the world";
			break;
		default:
			break;
	}
}

static int WarriorArtifacts[] = {
	IMPROVED_HEARING,	// 0
	TRAP_AVOIDANCE,         // 1
	PIERCING_DAGGERS,       // 2
	DAGGER_RECOVERY,        // 3
	PUSH_BACK,              // 4
	FEAR_INDUCING,          // 5
	INCREASED_STUN,         // 6
	CHARGE_THROUGH          // 7
};

static int RogueArtifacts[] = {
	IMPROVED_HEARING,	// 0
	TRAP_AVOIDANCE,		// 1
	PIERCING_DAGGERS,	// 2
	DAGGER_RECOVERY,	// 3
	PUSH_BACK,		// 4
	FEAR_INDUCING,		// 5
	INCREASED_STUN,		// 6
	PHASE_IMPROVEMENT	// 7
};

Artifact *
artifact_create_random(Player *p, Uint8 level)
{
	int option = -1;
	if (p->stats.lvl >= 4)
		option = get_random(7);
	else if (p->stats.lvl >= 3)
		option = get_random(6);
	else
		option = get_random(5);

	int * artifactPool = NULL;
	if (p->class == ROGUE)
		artifactPool = RogueArtifacts;
	else
		artifactPool = WarriorArtifacts;

	Artifact *a = artifact_create(artifactPool[option]);
	a->level = level;
	return a;
}

void
artifact_add_price(Artifact *a, unsigned int price, SDL_Renderer *renderer)
{
	
	Sprite *sprite = sprite_create();
	sprite_load_text_texture(sprite, "GUI/SDS_8x8.ttf", 0, 8, 1);
	char priceLabel[9];
	m_sprintf(priceLabel, 9, "$%d", price);
	texture_load_from_text(sprite->textures[0],
			       priceLabel,
			       C_YELLOW,
			       C_BLACK,
			       renderer);

	sprite->dim = sprite->textures[0]->dim;

	a->price = price;
	a->priceSprite = sprite;
}

Sprite *
artifact_sprite_for(MagicalEffect effect)
{
	Sprite *sprite = sprite_create();
	Texture *t;
	switch (effect) {
		case IMPROVED_HEARING:
			t = texturecache_add("Items/Potion.png");
			sprite_set_texture(sprite, t, 0);
			sprite->clip = CLIP16(7*16, 4*16);
			break;
		case TRAP_AVOIDANCE:
			t = texturecache_add("Items/Boot.png");
			sprite_set_texture(sprite, t, 0);
			sprite->clip = CLIP16(5*16, 0);
			break;
		case PIERCING_DAGGERS:
			t = texturecache_add("Items/Rock.png");
			sprite_set_texture(sprite, t, 0);
			sprite->clip = CLIP16(0, 0);
			break;
		case CHARGE_THROUGH:
			t = texturecache_add("Items/Shield.png");
			sprite_set_texture(sprite, t, 0);
			sprite->clip = CLIP16(16, 0);
			break;
		case PUSH_BACK:
			t = texturecache_add("Items/Glove.png");
			sprite_set_texture(sprite, t, 0);
			sprite->clip = CLIP16(0, 0);
			break;
		case DAGGER_RECOVERY:
			t = texturecache_add("Items/LongWep.png");
			sprite_set_texture(sprite, t, 0);
			sprite->clip = CLIP16(0, 6*16);
			break;
		case INCREASED_STUN:
			t = texturecache_add("Items/Shield.png");
			sprite_set_texture(sprite, t, 0);
			sprite->clip = CLIP16(4*16, 0);
			break;
		case FEAR_INDUCING:
			t = texturecache_add("Items/Armor.png");
			sprite_set_texture(sprite, t, 0);
			sprite->clip = CLIP16(6*16, 8*16);
			break;
		case PHASE_IMPROVEMENT:
			t = texturecache_add("Items/Armor.png");
			sprite_set_texture(sprite, t, 0);
			sprite->clip = CLIP16(1*16, 5*16);
			break;
		default:
			break;
	}
	return sprite;
}

Artifact *
artifact_create(MagicalEffect effect)
{
	Artifact *a = ec_malloc(sizeof(Artifact));
	a->sprite = artifact_sprite_for(effect);
	a->priceSprite = NULL;
	a->sprite->dim = GAME_DIMENSION;
	a->collected = false;
	a->level = 1;
	a->price = 0;
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
	Position pos = a->sprite->pos;
	pos.x += 4;
	pos.y += 4;
	particle_engine_sparkle(pos, (Dimension) { 24, 24 }, C_PURPLE, false);
	if (a->priceSprite)
		sprite_render(a->priceSprite, cam);
}

void
artifact_destroy(Artifact *a)
{
	sprite_destroy(a->sprite);
	if (a->priceSprite)
		sprite_destroy(a->priceSprite);
	free(a);
}
