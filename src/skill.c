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
#include "item.h"
#include "animation.h"
#include "artifact.h"
#include "trap.h"
#include "tooltip.h"

static char *flurry_tooltip[] = {
	"FLURRY", "",
	"",
	"   Hits an adjecant enemy with a flurry of three strikes.", "",
	"   Each strike has the same odds of hitting as a regular attack", "",
	"",
	"COOLDOWN:", "",
	"   5 turns", "",
	"",
	"USAGE:", "",
	"   activate the skill (press ", "1", ")", "",
	"   followed by a direction (left, right, up or down)", "",
	"",
	"",
	"Press ", "ESC", " to close", "", "",
	NULL
};

static char *bash_tooltip[] = {
	"BASH", "",
	"",
	"   Bashes an adjecant enemy with your shield", "",
	"   On a successful hit the target will be stunned for 2 turns", "",
	"",
	"COOLDOWN:", "",
	"   3 turns", "",
	"",
	"USAGE:", "",
	"   activate the skill (press ", "2", ")", "",
	"   followed by a direction (left, right, up or down)", "",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static char *trip_tooltip[] = {
	"TRIP", "",
	"",
	"   Trips an adjecant enemy causing him to fall (move), in", "",
	"   the direction you tripped it in.", "",
	"   On a successful hit the enemy will also be stunned." "",
	"",
	"   This can be combined with traps and pits to great effect.", "",
	"",
	"COOLDOWN:", "",
	"   3 turns", "",
	"",
	"USAGE:", "",
	"   activate the skill (press ", "2", ")", "",
	"   followed by a direction (left, right, up or down)", "",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static char *backstab_tooltip[] = {
	"BACKSTAB", "",
	"",
	"   You flip over an adjecant enemy taking it's place and", "",
	"   it taking yours, finnishing off with a stab in the back", "",
	"   of your foe.", "",
	"   A successful attack will also leave the enemy stunned.", "",
	"",
	"COOLDOWN:", "",
	"   5 turns", "",
	"",
	"USAGE:", "",
	"   activate the skill (press ", "1", ")", "",
	"   followed by a direction (left, right, up or down)", "",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static char *phase_tooltip[] = {
	"PHASE", "",
	"",
	"   You phase out of existence for a time. While you are phased you", "",
	"   are unaffected by gravity, traps and enemies won't see you.", "",
	"   You can also pass through enemies.", "",
	"",
	"   The effect lasts for 3 turns", "",
	"",
	"COOLDOWN:", "",
	"   8 turns", "",
	"",
	"USAGE:",
	"   activate the skill (press ", "3", ")", "",
	"   then move as normal", "",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static char *charge_tooltip[] = {
	"CHARGE", "",
	"",
	"   You charge in a chosen direction into the first obstructing", "",
	"   object. Charging into an enemy can deliver massive damage.", "",
	"",
	"   Damage is affected by charge distance.", "",
	"   Longer distance, more damage.", "",
	"",
	"COOLDOWN:", "",
	"   5 turns", "",
	"",
	"USAGE:",
	"   activate the skill (press ", "3", ")", "",
	"   followed by a direction (left, right, up or down)", "",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static char *blink_tooltip[] = {
	"BLINK", "",
	"",
	"   You blink in a chosen direction into the first obstructing", "",
	"   object. Monsters will not obstruct your blink.", "",
	"",
	"COOLDOWN:", "",
	"   4 turns", "",
	"",
	"USAGE:",
	"   activate the skill (press ", "3", ")", "",
	"   followed by a direction (left, right, up or down)", "",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static char *erupt_tooltip[] = {
	"ERUPT", "",
	"",
	"   You erupt in a magical explosion damaging monsters", "",
	"   around you and causing bleeding.", "",
	"",
	"COOLDOWN:", "",
	"   3 turns", "",
	"",
	"USAGE:",
	"   Erupt (press ", "2", ")", "",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static char *dagger_tooltip[] = {
	"THROW DAGGER", "",
	"",
	"   You throw a dagger in the chosen direction.", "",
	"",
	"   Damage is affected by throwing distance.", "",
	"   Longer distance, more damage.", "",
	"",
	"   Dagger supply is not infinite, your current dagger", "",
	"   inventory is displayed in the panel to the right.", "",
	"",
	"COOLDOWN:", "",
	"   0 turns", "",
	"",
	"USAGE:", "",
	"   activate the skill (press ", "4", ")", "",
	"   followed by a direction (left, right, up or down)", "",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static char *health_tooltip[] = {
	"DRINK HEALTH", "",
	"",
	"   You take a sip from your health vial", "",
	"",
	"   The current amount of sips in your vials is", "",
	"   displayed in the panel to the right.", "",
	"",
	"COOLDOWN:", "",
	"   0 turns", "",
	"",
	"USAGE:", "",
	"   Sip health (press ", "5", ")", "",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static void
perform_pickups_for_space(RoomSpace *space, Player *player)
{
	// Pick up items in the path
	LinkedList *items = space->items;
	while (items != NULL) {
		Item *item = items->data;
		items = items->next;
		item_collected(item, player);
	}
	LinkedList *artifacts = space->artifacts;
	while (artifacts != NULL) {
		Artifact *artifact = artifacts->data;
		artifacts = artifacts->next;
		player_add_artifact(player, artifact);
	}
}

static void
handle_space_effects(RoomSpace *space,
		     Player *player)
{
	if (space->lethal)
		player_set_falling(player);
	else if (space->trap)
		trap_activate(space->trap, player);

	LinkedList *objects = space->objects;
	while (objects) {
		object_damage(objects->data, player);
		objects = objects->next;
	}
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
	skill->available = NULL;
	skill->use = NULL;
	skill->levelcap = 1;
	skill->tooltip = NULL;
	return skill;
}

static bool
check_skill_validity(Position *playerPos, Position *targetPos, SkillData *data)
{
	*playerPos = position_to_matrix_coords(&data->player->sprite->pos);
	*targetPos = *playerPos;
	targetPos->x += (int) data->direction.x;
	targetPos->y += (int) data->direction.y;

	player_turn(data->player, &data->direction);

	if (!position_in_roommatrix(targetPos)) {
		return false;
	}
	return true;
}

static bool
skill_use_flurry(Skill *skill, SkillData *data)
{
	UNUSED (skill);

	Position playerPos, targetPos;
	if (!check_skill_validity(&playerPos, &targetPos, data)) {
		return false;
	}

	animation_run(data->player->swordAnimation);
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

		data->player->stat_data.hits += hitCount;

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
	skill->levelcap = 2;
	skill->use = skill_use_flurry;
	return skill;
}

static bool
skill_throw_dagger_available(Player *player)
{
	return player->daggers > 0;
}

static bool
skill_throw_dagger(Skill *skill, SkillData *data)
{
	UNUSED(skill);

	if (data->player->daggers == 0)
		return false;

	data->player->daggers--;

	Projectile *p = projectile_dagger_create();
	if (vector2d_equals(VECTOR2D_UP, data->direction)) {
		p->velocity = (Vector2d) { 0, -DAGGER_VELOCITY };
		p->sprite->flip = SDL_FLIP_VERTICAL;
	}
	else if (vector2d_equals(VECTOR2D_DOWN, data->direction)) {
		p->velocity = (Vector2d) { 0, DAGGER_VELOCITY };
		p->sprite->flip = SDL_FLIP_HORIZONTAL;
	}
	else if (vector2d_equals(VECTOR2D_RIGHT, data->direction)) {
		p->velocity = (Vector2d) { DAGGER_VELOCITY, 0 };
		p->sprite->flip = SDL_FLIP_HORIZONTAL;
	}
	else {
		p->velocity = (Vector2d) { -DAGGER_VELOCITY, 0 };
		p->sprite->angle = -270;
	}

	player_turn(data->player, &data->direction);

	mixer_play_effect(SWOOSH);
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
	skill->levelcap = 1;
	skill->instantUse = false;
	skill->resetTime = 1;
	skill->available = skill_throw_dagger_available;
	skill->use = skill_throw_dagger;
	skill->actionRequired = false;
	return skill;
}

static bool
skill_bash(Skill *skill, SkillData *data)
{
	UNUSED (skill);

	Position playerPos, targetPos;
	if (!check_skill_validity(&playerPos, &targetPos, data)) {
		return false;
	}

	animation_run(data->player->swordAnimation);
	Monster *monster = data->matrix->spaces[targetPos.x][targetPos.y].monster;
	mixer_play_effect(SWING0);
	if (monster) {
		gui_log("You bash %s with your shield", monster->lclabel);
		unsigned int dmg = stats_fight(&data->player->stats, &monster->stats);
		if (dmg > 0) {
			gui_log("You hit for %u damage", dmg);
			if (monster->stats.hp > 0) {
				gui_log("%s seems dazed and confused", monster->label);
				monster_set_state(monster, STUNNED,
						  (Uint8) (3 + player_has_artifact(data->player, INCREASED_STUN)));
			}
			mixer_play_effect(SLAM);
			data->player->stat_data.hits += 1;
		} else {
			gui_log("You missed %s", monster->lclabel);
		}
		monster_hit(monster, dmg);
	} else {
		gui_log("You bash your shield at nothing");
	}
	player_monster_kill_check(data->player, monster);

	return true;
}

static Skill *
create_bash(void)
{
	Texture *t = texturecache_add("Extras/Skills.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = GAME_DIMENSION;
	s->clip = CLIP32(96, 0);
	s->fixed = true;
	Skill *skill = create_default("Bash", s);
	skill->levelcap = 3;
	skill->instantUse = false;
	skill->resetTime = 3;
	skill->available = NULL;
	skill->use = skill_bash;
	skill->actionRequired = true;
	return skill;
}

static bool
skill_trip(Skill *skill, SkillData *data)
{
	UNUSED (skill);

	Position playerPos, targetPos;
	if (!check_skill_validity(&playerPos, &targetPos, data)) {
		return false;
	}

	RoomSpace *space = &data->matrix->spaces[targetPos.x][targetPos.y];
	mixer_play_effect(SWING0 + get_random(2));
	animation_run(data->player->swordAnimation);
	if (space->monster) {
		int dmg = stats_fight(&data->player->stats, &space->monster->stats);
		if (dmg)
			mixer_play_effect(SWORD_HIT);
		gui_log("You trip %s causing it to fall away from you", space->monster->lclabel);
		monster_hit(space->monster, dmg);
		player_monster_kill_check(data->player, space->monster);
		if (dmg && space->monster->stats.hp > 0) {
			Uint32 pushCount = 1 + player_has_artifact(data->player, PUSH_BACK);
			for (Uint32 i = 0; i < pushCount; ++i) {
				monster_push(space->monster, data->player, data->matrix, data->direction);
				if (space->monster->stats.hp <= 0 || space->monster->sprite->state == SPRITE_STATE_FALLING) {
					break;
				}
			}
			monster_set_state(space->monster, STUNNED, (Uint8)(2 + player_has_artifact(data->player, INCREASED_STUN)));
		}


	} else {
		gui_log("You flail at the air");
	}
	return true;
}

static Skill *
create_trip(void)
{
	Texture *t = texturecache_add("Extras/Skills.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = GAME_DIMENSION;
	s->clip = CLIP32(32, 32);
	s->fixed = true;
	Skill *skill = create_default("Trip", s);
	skill->levelcap = 3;
	skill->instantUse = false;
	skill->resetTime = 2;
	skill->available = NULL;
	skill->use = skill_trip;
	skill->actionRequired = true;
	return skill;
}

static bool
skill_backstab(Skill *skill, SkillData *data)
{
	UNUSED(skill);

	Position playerPos, targetPos;
	if (!check_skill_validity(&playerPos, &targetPos, data)) {
		return false;
	}
	RoomSpace *targetSpace = &data->matrix->spaces[targetPos.x][targetPos.y];
	if (targetSpace->occupied) {
		return false;
	}

	Vector2d reverseDirection = data->direction;
	reverseDirection.x *= -1;
	reverseDirection.y *= -1;

	mixer_play_effect(SWING0 + get_random(2));

	data->player->sprite->pos.x += (int) data->direction.x * TILE_DIMENSION;
	data->player->sprite->pos.y += (int) data->direction.y * TILE_DIMENSION;
	player_turn(data->player, &reverseDirection);
	animation_run(data->player->swordAnimation);

	if (targetSpace->monster) {
		Monster *m = targetSpace->monster;
		monster_push(m, data->player, data->matrix, reverseDirection);

		m->stats.disadvantage = true;
		int dmg = stats_fight(&data->player->stats, &m->stats);
		m->stats.disadvantage = false;
		monster_hit(m, dmg);
		player_monster_kill_check(data->player, m);
		if (dmg) {
			mixer_play_effect(SWORD_HIT);
			monster_set_state(m, STUNNED, (Uint8)(2 + player_has_artifact(data->player, INCREASED_STUN)));
			monster_set_bleeding(m);
		}
	}

	perform_pickups_for_space(targetSpace, data->player);
	handle_space_effects(targetSpace, data->player);

	return true;
}

static Skill *
create_backstab(void)
{
	Texture *t = texturecache_add("Extras/Skills.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = GAME_DIMENSION;
	s->clip = CLIP32(0, 32);
	s->fixed = true;
	Skill *skill = create_default("Backstab", s);
	skill->levelcap = 2;
	skill->instantUse = false;
	skill->resetTime = 2;
	skill->available = NULL;
	skill->use = skill_backstab;
	skill->actionRequired = true;
	return skill;
}

static bool
skill_phase(Skill *skill, SkillData *data)
{
	UNUSED(skill);
	mixer_play_effect(FADE_OUT);
	data->player->phase_count = 3;
	return true;
}

static Skill *
create_phase(void)
{
	Texture *t = texturecache_add("Extras/Skills.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = GAME_DIMENSION;
	s->clip = CLIP32(96, 32);
	s->fixed = true;
	Skill *skill = create_default("Phase", s);
	skill->levelcap = 4;
	skill->instantUse = true;
	skill->resetTime = 6;
	skill->available = NULL;
	skill->use = skill_phase;
	skill->actionRequired = false;
	return skill;
}

static bool
skill_sip_health_available(Player *player)
{
	return player->potion_sips > 0 && player->stats.hp != player->stats.maxhp;
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
	skill->levelcap = 1;
	skill->instantUse = true;
	skill->available = skill_sip_health_available;
	skill->use = skill_sip_health;
	skill->resetTime = 0;
	return skill;
}

static void
skill_charge_check_path(SkillData *data,
			Position origin,
			Position dest)
{
	RoomMatrix *matrix = data->matrix;
	Player *player = data->player;

	Position itPos = origin;
	Position lastPos = dest;
	lastPos.x += (int) data->direction.x * 2;
	lastPos.y += (int) data->direction.y * 2;
	Uint8 steps = 1;
	while (position_in_roommatrix(&itPos) && !position_equals(&itPos, &lastPos)) {
		RoomSpace *space = &matrix->spaces[itPos.x][itPos.y];
		if (space->monster) {
			Monster *monster = matrix->spaces[itPos.x][itPos.y].monster;
			Stats tmpStats = player->stats;
			tmpStats.dmg *= steps > 0 ? steps : 1;
			mixer_play_effect(SWING0 + get_random(2));
			unsigned int dmg = stats_fight(&tmpStats, &monster->stats);
			if (dmg > 0) {
				gui_log("You charged %s for %u damage", monster->lclabel, dmg);
				mixer_play_effect(SWORD_HIT);
				data->player->stat_data.hits += 1;
			}
			monster_hit(monster, dmg);
			player_monster_kill_check(data->player, monster);
		}

		perform_pickups_for_space(space, player);

		if (space->trap)
			space->trap->sprite->animate = true;

		itPos.x += (int) data->direction.x;
		itPos.y += (int) data->direction.y;

		steps++;
	}
}

static bool
skill_charge(Skill *skill, SkillData *data)
{
	UNUSED(skill);

	Player *player = data->player;
	RoomMatrix *matrix = data->matrix;

	Position playerStartPos = position_to_matrix_coords(&player->sprite->pos);
	Position destination = playerStartPos;

	// Find collider
	destination.x += (int) data->direction.x;
	destination.y += (int) data->direction.y;
	RoomSpace *space = &matrix->spaces[destination.x][destination.y];
	Uint32 passThroughCount = 0;
	Uint32 chargeThroughLvl = player_has_artifact(data->player, CHARGE_THROUGH);
	Position lastAvailableDest = playerStartPos;
	while (position_in_roommatrix(&destination))
	{
		if (space->occupied || space->monster) {
			if (!space->monster || passThroughCount >= chargeThroughLvl)
				break;
			else
				passThroughCount++;
		} else {
			lastAvailableDest = destination;
		}

		destination.x += (int) data->direction.x;
		destination.y += (int) data->direction.y;
		space = &matrix->spaces[destination.x][destination.y];
	}

	destination = lastAvailableDest;

	// Move player
	Position playerOriginPos = player->sprite->pos;
	Sint32 xdiff = destination.x - playerStartPos.x;
	Sint32 ydiff = destination.y - playerStartPos.y;
	player->sprite->pos.x += xdiff * TILE_DIMENSION;
	player->sprite->pos.y += ydiff * TILE_DIMENSION;
	Position playerDestinationPos = player->sprite->pos;
	player_turn(data->player, &data->direction);

	// Add motion particles
	bool horizontal = data->direction.x != 0;
	Dimension particleArea;
	if (horizontal)
		particleArea = (Dimension) { abs(xdiff) * TILE_DIMENSION, TILE_DIMENSION };
	else
		particleArea = (Dimension) { TILE_DIMENSION, abs(ydiff) * TILE_DIMENSION };

	Position speedLinePos;
	if (playerOriginPos.x < playerDestinationPos.x || playerOriginPos.y < playerDestinationPos.y)
		speedLinePos = playerOriginPos;
	else
		speedLinePos = playerDestinationPos;

	particle_engine_speed_lines(speedLinePos, particleArea, horizontal);
	mixer_play_effect(SWOOSH);

	skill_charge_check_path(data, playerStartPos, destination);

	Position lastTilePos = position_to_matrix_coords(&playerDestinationPos);
	RoomSpace *destSpace = &matrix->spaces[lastTilePos.x][lastTilePos.y];
	handle_space_effects(destSpace, player);

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
	skill->levelcap = 4;
	skill->use = skill_charge;
	return skill;
}

static bool
skill_blink(Skill *skill, SkillData *data)
{
	UNUSED(skill);

	Player *player = data->player;
	RoomMatrix *matrix = data->matrix;

	Position playerStartPos = position_to_matrix_coords(&player->sprite->pos);
	Position destination = playerStartPos;

	// Find collider
	destination.x += (int) data->direction.x;
	destination.y += (int) data->direction.y;
	RoomSpace *space = &matrix->spaces[destination.x][destination.y];
	Position lastAvailableDest = playerStartPos;
	while (position_in_roommatrix(&destination))
	{
		if (space->occupied) {
			break;
		}

		if (!space->monster) {
			lastAvailableDest = destination;
		}

		destination.x += (int) data->direction.x;
		destination.y += (int) data->direction.y;
		space = &matrix->spaces[destination.x][destination.y];
	}

	destination = lastAvailableDest;

	// Move player
	Position playerOriginPos = player->sprite->pos;
	Sint32 xdiff = destination.x - playerStartPos.x;
	Sint32 ydiff = destination.y - playerStartPos.y;
	player->sprite->pos.x += xdiff * TILE_DIMENSION;
	player->sprite->pos.y += ydiff * TILE_DIMENSION;
	Position playerDestinationPos = player->sprite->pos;
	player_turn(data->player, &data->direction);

	particle_engine_blink(playerOriginPos, DIM(32, 32));
	particle_engine_blink(playerDestinationPos, DIM(32, 32));
	mixer_play_effect(BLINK_EFFECT);

	Position lastTilePos = position_to_matrix_coords(&playerDestinationPos);
	RoomSpace *destSpace = &matrix->spaces[lastTilePos.x][lastTilePos.y];

	perform_pickups_for_space(destSpace, player);
	handle_space_effects(destSpace, player);

	return true;
}

static Skill *
create_blink(void)
{
	Texture *t = texturecache_add("Extras/Skills.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = GAME_DIMENSION;
	s->clip = CLIP32(64, 64);
	s->fixed = true;
	Skill *skill = create_default("Blink", s);
	skill->levelcap = 3;
	skill->use = skill_blink;
	skill->resetTime = 4;
	return skill;
}

static bool
skill_erupt(Skill *skill, SkillData *data)
{
	UNUSED(skill);

	Player *player = data->player;
	RoomMatrix *rm = data->matrix;

	gui_log("You erupt in a magical explosion");
	particle_engine_eldritch_explosion(player->sprite->pos, DIM(32, 32));
	mixer_play_effect(BLAST_EFFECT);

	Position playerMPos = position_to_matrix_coords(&player->sprite->pos);
	int range = player_has_artifact(player, SKILL_RADIUS);
	for (Sint32 i = -1 - range; i <= 1 + range; ++i) {
		for (Sint32 j = -1 - range; j <= 1 + range; ++j) {

			if (i == 0 && j == 0)
				continue;

			Position matrixPos = POS(playerMPos.x + i, playerMPos.y + j);
			if (!position_in_roommatrix(&matrixPos))
				continue;

			RoomSpace *r = &rm->spaces[matrixPos.x][matrixPos.y];
			if (r->monster) {
				player->stats.advantage = true;
				int dmg = stats_fight(&player->stats, &r->monster->stats);
				player->stats.advantage = false;
				monster_hit(r->monster, dmg);
				gui_log("%s takes %d damage from the explosion", r->monster->label, dmg);
				monster_set_bleeding(r->monster);

				int lvl = 1 + player_has_artifact(player, PUSH_BACK);
				monster_push(r->monster,
					     player,
					     rm,
					     VEC2D((float) (i*lvl),
						   (float) (j*lvl))
					    );
			}
		}
	}

	return true;
}

static Skill *
create_erupt(void)
{
	Texture *t = texturecache_add("Extras/Skills.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = GAME_DIMENSION;
	s->clip = CLIP32(32, 64);
	s->fixed = true;
	Skill *skill = create_default("Erupt", s);
	skill->levelcap = 3;
	skill->use = skill_erupt;
	skill->instantUse = true;
	skill->resetTime = 3;
	return skill;
}

Skill*
skill_create(enum SkillType t, Camera *cam)
{
	Skill *skill;
	switch (t) {
		case FLURRY:
			skill = create_flurry();
			skill->tooltip = tooltip_create(flurry_tooltip, cam);
			break;
		case SIP_HEALTH:
			skill = create_sip_health();
			skill->tooltip = tooltip_create(health_tooltip, cam);
			break;
		case CHARGE:
			skill = create_charge();
			skill->tooltip = tooltip_create(charge_tooltip, cam);
			break;
		case BLINK:
			skill = create_blink();
			skill->tooltip = tooltip_create(blink_tooltip, cam);
			break;
		case ERUPT:
			skill = create_erupt();
			skill->tooltip = tooltip_create(erupt_tooltip, cam);
			break;
		case DAGGER_THROW:
			skill = create_throw_dagger();
			skill->tooltip = tooltip_create(dagger_tooltip, cam);
			break;
		case BASH:
			skill = create_bash();
			skill->tooltip = tooltip_create(bash_tooltip, cam);
			break;
		case TRIP:
			skill = create_trip();
			skill->tooltip = tooltip_create(trip_tooltip, cam);
			break;
		case BACKSTAB:
			skill = create_backstab();
			skill->tooltip = tooltip_create(backstab_tooltip, cam);
			break;
		case PHASE:
			skill = create_phase();
			skill->tooltip = tooltip_create(phase_tooltip, cam);
			break;
		default:
			fatal("Unknown SkillType %u", (unsigned int) t);
			return NULL;
	}

#ifdef DEBUG
	skill->levelcap = 1;
#endif
	return skill;
}

void
skill_destroy(Skill *skill)
{
	sprite_destroy(skill->icon);
	if (skill->tooltip)
		sprite_destroy(skill->tooltip);
	free(skill);
}
