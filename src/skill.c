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
#include "texturecache.h"
#include "skill.h"
#include "util.h"
#include "player.h"
#include "roommatrix.h"
#include "stats.h"
#include "monster.h"
#include "mixer.h"
#include "gui.h"
#include "random.h"
#include "particle_engine.h"
#include "projectile.h"
#include "linkedlist.h"

static void
set_player_clip_for_direction(Player *player, Vector2d *direction)
{
	if (direction->y > 0) // Down
		player->sprite->clip.y = 0;
	else if (direction->y < 0)  // Up
		player->sprite->clip.y = 48;
	else if (direction->x < 0)  // Left
		player->sprite->clip.y = 16;
	else if (direction->x > 0)  // Right
		player->sprite->clip.y = 32;
}

static Skill *
create_default(const char *s_label, Sprite *s)
{
	Skill *skill = ec_malloc(sizeof(Skill));
	m_strcpy(skill->label, 20, s_label);
	skill->resetTime = 5;
	skill->resetCountdown = 0;
	skill->icon = s;
	skill->actionRequired = true;
	skill->instantUse = false;
	skill->active = false;
	skill->use = NULL;
	return skill;
}

static bool
skill_use_flurry(Skill *skill, SkillData *data)
{
	UNUSED (skill);

	Position playerPos = position_to_matrix_coords(&data->player->sprite->pos);
	Position targetPos = playerPos;
	targetPos.x += (int) data->direction.x;
	targetPos.y += (int) data->direction.y;

	set_player_clip_for_direction(data->player, &data->direction);

	if (!position_in_roommatrix(&targetPos)) {
		return false;
	}

	Monster *monster = data->matrix->spaces[targetPos.x][targetPos.y].monster;
	mixer_play_effect(TRIPPLE_SWING);
	if (monster) {
		gui_log("You attack %s with a flurry of strikes", monster->lclabel);
		unsigned int hitCount = 0;
		for (size_t i = 0; i < 3; ++i) {
			unsigned int originalHp = monster->stats.hp;
			unsigned int dmg = stats_fight(&data->player->stats, &monster->stats);
			if (dmg > 0 && originalHp > 0) {
				gui_log("You hit for %u damage", dmg);
				hitCount++;
			}
			monster_hit(monster, dmg);
		}
		if (hitCount == 1) {
			mixer_play_effect(SWORD_HIT);
		} else if (hitCount == 2) {
			mixer_play_effect(DOUBLE_SWORD_HIT);
		} else if (hitCount == 3) {
			mixer_play_effect(TRIPPLE_SWORD_HIT);
		}

		data->player->hits += hitCount;

	} else {
		gui_log("You swing at thin air with a flurry of strikes");
	}
	player_monster_kill_check(data->player, monster);

	return true;
}

static Skill *
create_flurry(void)
{
	Texture *t = texturecache_add("Extras/Skills.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = GAME_DIMENSION;
	s->clip = CLIP32(0, 0);
	s->fixed = true;
	Skill *skill = create_default("Flurry", s);
	skill->use = skill_use_flurry;
	return skill;
}

static bool
skill_throw_dagger(Skill *skill, SkillData *data)
{
	UNUSED(skill);
	UNUSED(data);

	Projectile *p = projectile_dagger_create();
	if (vector2d_equals(VECTOR2D_UP, data->direction))
		p->velocity = (Vector2d) { 0, -300 };
	else if (vector2d_equals(VECTOR2D_DOWN, data->direction))
		p->velocity = (Vector2d) { 0, 300 };
	else if (vector2d_equals(VECTOR2D_RIGHT, data->direction))
		p->velocity = (Vector2d) { 300, 0 };
	else
		p->velocity = (Vector2d) { -300, 0 };

	p->sprite->pos = data->player->sprite->pos;
	linkedlist_append(&data->player->projectiles, p);

	return true;
}

static Skill *
create_throw_dagger(void)
{
	Texture *t = texturecache_add("Extras/Skills.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = GAME_DIMENSION;
	s->clip = CLIP32(64, 0);
	s->fixed = true;
	Skill *skill = create_default("Throw dagger", s);
	skill->instantUse = false;
	skill->use = skill_throw_dagger;
	skill->actionRequired = false;
	return skill;
}

static bool
skill_sip_health(Skill *skill, SkillData *data)
{
	UNUSED (skill);
	player_sip_health(data->player);
	return true;
}

static Skill *
create_sip_health(void)
{
	Texture *t = texturecache_add("Items/Potion.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = DEFAULT_DIMENSION;
	s->clip = CLIP16(0, 0);
	s->fixed = true;
	Skill *skill = create_default("Sip health", s);
	skill->instantUse = true;
	skill->use = skill_sip_health;
	skill->resetTime = 0;
	return skill;
}

static bool
skill_charge(Skill *skill, SkillData *data)
{
	UNUSED(skill);

	Player *player = data->player;
	RoomMatrix *matrix = data->matrix;

	Position playerPos = position_to_matrix_coords(&player->sprite->pos);
	Position destination = playerPos;

	unsigned int steps = 0;

	// Find collider
	destination.x += (int) data->direction.x;
	destination.y += (int) data->direction.y;
	while (position_in_roommatrix(&destination)
		&& !matrix->spaces[destination.x][destination.y].occupied)
	{
		destination.x += (int) data->direction.x;
		destination.y += (int) data->direction.y;
		steps++;
	}

	if (!position_in_roommatrix(&destination)) {
		destination.x -= (int) data->direction.x;
		destination.y -= (int) data->direction.y;
	}

	// Move player
	Position playerOriginPos = player->sprite->pos;
	player->sprite->pos.x += (steps * TILE_DIMENSION) * (int) data->direction.x;
	player->sprite->pos.y += (steps * TILE_DIMENSION) * (int) data->direction.y;
	Position playerDestinationPos = player->sprite->pos;
	set_player_clip_for_direction(data->player, &data->direction);

	// Add motion particles
	bool horizontal = data->direction.x != 0;
	Dimension particleArea;
	if (horizontal)
		particleArea = (Dimension) { steps * TILE_DIMENSION, TILE_DIMENSION };
	else
		particleArea = (Dimension) { TILE_DIMENSION, steps * TILE_DIMENSION };

	Position speedLinePos;
	if (playerOriginPos.x < playerDestinationPos.x || playerOriginPos.y < playerDestinationPos.y)
		speedLinePos = playerOriginPos;
	else
		speedLinePos = playerDestinationPos;

	particle_engine_speed_lines(speedLinePos, particleArea, horizontal);
	mixer_play_effect(SWOOSH);

	if (matrix->spaces[destination.x][destination.y].monster) {
		Monster *monster = matrix->spaces[destination.x][destination.y].monster;
		Stats tmpStats = player->stats;
		tmpStats.dmg *= steps > 0 ? steps : 1;
		mixer_play_effect(SWING0 + get_random(3) - 1);
		unsigned int dmg = stats_fight(&tmpStats, &monster->stats);
		if (dmg > 0) {
			gui_log("You charged %s for %u damage", monster->lclabel, dmg);
			mixer_play_effect(SWORD_HIT);
			data->player->hits += 1;
		}
		monster_hit(monster, dmg);
	}

	return true;
}

static Skill *
create_charge(void)
{
	Texture *t = texturecache_add("Extras/Skills.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = GAME_DIMENSION;
	s->clip = CLIP32(32, 0);
	s->fixed = true;
	Skill *skill = create_default("Charge", s);
	skill->use = skill_charge;
	return skill;
}

Skill*
skill_create(enum SkillType t)
{
	switch (t) {
		case FLURRY:
			return create_flurry();
		case SIP_HEALTH:
			return create_sip_health();
		case CHARGE:
			return create_charge();
		case DAGGER_THROW:
			return create_throw_dagger();
		default:
			fatal("Unknown SkillType %u", (unsigned int) t);
			return NULL;
	}
}

void
skill_destroy(Skill *skill)
{
	sprite_destroy(skill->icon);
	free(skill);
}
