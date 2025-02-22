/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2025  Linus Probert <linus.probert@gmail.com>
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
#pragma once

#include "sprite.h"
#include "camera.h"

typedef enum MagicalEffect {
	IMPROVED_HEARING,
	TRAP_AVOIDANCE,
	PIERCING_DAGGERS,
	DAGGER_RECOVERY,
	PUSH_BACK,
	FEAR_INDUCING,
	INCREASED_STUN,
	CHARGE_THROUGH,
	PHASE_IMPROVEMENT,
	SKILL_RADIUS,
	DAGGER_BOUNCE,
	EXPLOSIVE_KILLS,
	VOLATILE_DAGGERS,
	LAST_ARTIFACT_EFFECT // Sentinel
} MagicalEffect;

typedef struct ArtifactInfo {
	const char *name;
	const char *desc;
} ArtifactInfo;

typedef struct Artifact {
	Sprite *sprite;
	Sprite *priceSprite;
	Sprite *levelSprite;
	MagicalEffect effect;
	ArtifactInfo info;
	bool collected;
	int level;
	unsigned int price;
} Artifact;

Sprite *
artifact_sprite_for(MagicalEffect);

Artifact *
artifact_create_random(Player*, Uint8 level);

void
artifact_add_price(Artifact*, unsigned int price);

Artifact *
artifact_create(MagicalEffect);

Artifact *
artifact_copy(const Artifact*);

void
artifact_render(Artifact*, Camera*);

void
artifact_destroy(Artifact*);
