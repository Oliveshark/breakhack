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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "player.h"
#include "monster.h"
#include "util.h"
#include "gui.h"
#include "item.h"
#include "particle_engine.h"
#include "keyboard.h"
#include "mixer.h"
#include "random.h"
#include "projectile.h"
#include "texturecache.h"
#include "vector2d.h"
#include "actiontextbuilder.h"
#include "animation.h"
#include "trap.h"
#include "gamecontroller.h"
#include "event.h"
#include "effect_util.h"

#ifdef STEAM_BUILD
#include "steam/steamworks_api_wrapper.h"
#endif // STEAM_BUILD

#define ENGINEER_STATS	{ 12, 12, 5, 7, 2, 1, 1, false, false }
#define MAGE_STATS	{ 12, 12, 5, 7, 1, 1, 1, false, false }
#define PALADIN_STATS	{ 12, 12, 8, 9, 3, 1, 1, false, false }
#define ROGUE_STATS	{  9,  9, 6, 9, 4, 2, 1, false, false }
#define WARRIOR_STATS	{ 12, 12, 8, 9, 3, 1, 1, false, false }

static void
levelup(Player *player)
{
	player->stats.lvl += 1;
	player->stats.maxhp += 9;
	player->stats.dmg += 5;
	player->stats.atk += 1;

	// Limit health to 3 rows of hearts
	if (player->stats.maxhp > 72)
		player->stats.maxhp = 72;

	player->stats.hp = player->stats.maxhp;
}

void
player_levelup(Player *player)
{
	mixer_play_effect(LEVEL_UP);
	levelup(player);
}

void
player_set_level(Player *p, Uint8 level)
{
	while (p->stats.lvl < level)
		levelup(p);
}

static unsigned int
next_level_threshold(unsigned int current_level)
{
	unsigned int last_level = 0;
	unsigned int padding = 0;

	if (current_level > 0) {
		last_level = next_level_threshold(current_level - 1);
		padding = (current_level - 1) * 150;
	}

	return last_level + (current_level * 50) + padding;
}

static void
player_gain_xp(Player *player, unsigned int xp_gain)
{
	char msg[10];
	m_sprintf(msg, 10, "+%dxp", xp_gain);
	actiontextbuilder_create_text(msg, C_GREEN, &player->sprite->pos);

	player->xp += xp_gain;
	if (player->xp >= next_level_threshold(player->stats.lvl)) {
		player_levelup(player);
		gui_log("You have reached level %u", player->stats.lvl);
		gui_event_message("You reached level %u", player->stats.lvl);
		actiontextbuilder_create_text("Level up", C_GREEN, &player->sprite->pos);
	}
}

static void
action_spent(Player *p)
{
	p->stat_data.steps++;
	p->stat_data.total_steps++;

	if (p->stat_data.steps >= p->stats.speed) {
		for (size_t i = 0; i < PLAYER_SKILL_COUNT; ++i) {
			if (p->skills[i] != NULL && p->skills[i]->resetCountdown > 0)
				p->skills[i]->resetCountdown--;
		}
		if (p->phase_count > 0) {
			p->phase_count--;
			if (!p->phase_count) {
				mixer_play_effect(FADE_IN);
			}
		}
	}
}

static void
on_monster_collision(Player *player,
		     Monster *monster,
		     RoomMatrix *matrix,
		     Vector2d direction)
{
	CombatResult result = stats_fight(&player->stats,
					  &monster->stats);

	mixer_play_effect(SWING0 + get_random(2));
	monster_hit(monster, result.dmg, result.critical);
	animation_run(player->swordAnimation);

	if (result.dmg > 0) {
		gui_log("You hit %s for %u damage",
			monster->lclabel, result.dmg);
		player->stat_data.hits += 1;
		mixer_play_effect(SWORD_HIT);
	} else {
		gui_log("You missed %s", monster->lclabel);
		player->stat_data.misses += 1;
	}
	player_monster_kill_check(player, monster);
	if (monster->stats.hp <= 0 && (player_has_artifact(player, EXPLOSIVE_KILLS))) {
		mixer_play_effect(EXPLOSION_EFFECT);
		particle_engine_fire_explosion(monster->sprite->pos, DIM(32, 32));
		effect_damage_surroundings(&monster->sprite->pos,
					   matrix,
					   player,
					   &player->stats,
					   player_has_artifact(player, EXPLOSIVE_KILLS),
					   0,
					   false);
	}

	if (monster->stats.hp > 0) {
		if (get_random(10) < player_has_artifact(player, PUSH_BACK)) {
			gui_log("The force of your blow sends %s reeling",
				monster->lclabel);
			monster_push(monster, player, matrix, direction);
		}

		if (get_random(10) < player_has_artifact(player, FEAR_INDUCING)) {
			gui_log("%s shivers with fear at the sight of you",
				monster->label);
			if (monster->state.current != STUNNED)
				monster_set_state(monster, SCARED, 3);
		}
	}

	action_spent(player);
}

	static void
player_pickup_artifacts(Player *player, RoomSpace *space)
{
	LinkedList *artifacts = space->artifacts;
	while (artifacts) {
		player_add_artifact(player, artifacts->data);
		artifacts = artifacts->next;
	}
}

	static void
player_interact_objects(Player *player, RoomSpace *space)
{
	LinkedList *objects = space->objects;
	while (objects) {
		object_damage(objects->data, player);
		objects = objects->next;
	}
}

	static void
player_collect_items(Player *player, RoomSpace *space)
{
	LinkedList *items = space->items;
	while (items != NULL) {
		Item *item = items->data;
		items = items->next;
		item_collected(item, player);
	}
}

	static void
player_interact_traps_and_pits(Player *player, RoomSpace *space)
{
	if (space->lethal) {
		player_set_falling(player);
	}

	if (space->trap) {
		trap_activate(space->trap, player);
	}
}

	static bool
player_has_collided(Player *p, RoomSpace *space)
{
	if (space->occupied)
		return true;

	return !p->phase_count && space->monster && space->monster->sprite->state != SPRITE_STATE_FALLING;
}

	static bool 
has_collided(Player *player, RoomMatrix *matrix, Vector2d direction)
{
	Position roomCoord = position_to_room_coords(&player->sprite->pos);
	if (roomCoord.x != matrix->roomPos.x || roomCoord.y != matrix->roomPos.y) {
		return false;
	}

	Position matrixPos = position_to_matrix_coords(&player->sprite->pos);
	RoomSpace *space = &matrix->spaces[matrixPos.x][matrixPos.y];

	if (player_has_collided(player, space)) {
		player->sprite->pos.x -= TILE_DIMENSION * (int)direction.x;
		player->sprite->pos.y -= TILE_DIMENSION * (int)direction.y;

		gamecontroller_rumble(0.30f, 100);
		if (space->monster) {
			on_monster_collision(player, space->monster, matrix, direction);
		} else if (space->door && map_open_door(space->door, player)) {
			// Door opened, pass
		} else {
			mixer_play_effect(BONK);
			camera_shake(direction, 100);
			if (space->door)
				gui_log("Ouch! That door seems to be locked");
			else
				gui_log("Ouch! There is something in the way");
		}
		return true;
	}
	else {
		player_collect_items(player, space);
		player_pickup_artifacts(player, space);

		// If not phased or phase will end this turn, react to traps and pits
		if (!player->phase_count || (player->phase_count == 1 && player->stats.speed == (player->stat_data.steps + 1))) {
			player_interact_objects(player, space);
			player_interact_traps_and_pits(player, space);
		}
		return false;
	}
}

	static void
set_clip_for_direction(Player *player, Vector2d *direction)
{
	if (vector2d_equals(*direction, VECTOR2D_LEFT))
		player->sprite->clip.y = 16;
	else if (vector2d_equals(*direction, VECTOR2D_RIGHT))
		player->sprite->clip.y = 32;
	else if (vector2d_equals(*direction, VECTOR2D_UP))
		player->sprite->clip.y = 48;
	else if (vector2d_equals(*direction, VECTOR2D_DOWN))
		player->sprite->clip.y = 0;
}

	void
player_turn(Player *player, Vector2d *direction)
{
	set_clip_for_direction(player, direction);

	if (!vector2d_equals(*direction, VECTOR2D_NODIR))
		player->swordAnimation->sprite->pos = player->sprite->pos;

	if (vector2d_equals(*direction, VECTOR2D_UP)) {
		player->swordAnimation->sprite->pos.y -= 32;
		player->swordAnimation->sprite->angle = -90;
		player->swordAnimation->sprite->flip = SDL_FLIP_NONE;
	} else if (vector2d_equals(*direction, VECTOR2D_DOWN)) {
		player->swordAnimation->sprite->pos.y += 32;
		player->swordAnimation->sprite->angle = 90;
		player->swordAnimation->sprite->flip = SDL_FLIP_NONE;
	} else if (vector2d_equals(*direction, VECTOR2D_LEFT)) {
		player->swordAnimation->sprite->pos.x -= 32;
		player->swordAnimation->sprite->angle = 0;
		player->swordAnimation->sprite->flip = SDL_FLIP_HORIZONTAL;
	} else if (vector2d_equals(*direction, VECTOR2D_RIGHT)) {
		player->swordAnimation->sprite->pos.x += 32;
		player->swordAnimation->sprite->angle = 0;
		player->swordAnimation->sprite->flip = SDL_FLIP_NONE;
	}
}

	static void
move(Player *player, RoomMatrix *matrix, Vector2d direction)
{
	player_turn(player, &direction);

	Position lastPos = position_to_matrix_coords(&player->sprite->pos);

	player->sprite->pos.x += TILE_DIMENSION * (int) direction.x;
	player->sprite->pos.y += TILE_DIMENSION * (int) direction.y;

	if (!has_collided(player, matrix, direction)) {
		action_spent(player);
		RoomSpace *lastSpace = &matrix->spaces[lastPos.x][lastPos.y];
		if (matrix->modifier->type == RMOD_TYPE_CRUMBLING &&
		    lastPos.x > 1 &&
		    lastPos.y > 1 &&
		    lastPos.x < 14 &&
		    lastPos.y < 10 &&
		    lastSpace->decoration == NULL &&
		    lastSpace->trap == NULL &&
		    lastSpace->objects == NULL &&
		    lastSpace->items == NULL
		   )
			map_trigger_tile_fall(lastSpace->tile);
	}
}

	void
player_sip_health(Player *player)
{
	bool hasSips = player->class == MAGE ?
		player->potion_sips > 1 : player->potion_sips > 0;

	if (hasSips) {
		--player->potion_sips;
		if (player->class == MAGE)
			--player->potion_sips;
		++player->stats.hp;
		mixer_play_effect(BUBBLE0 + get_random(2));
		gui_log("You take a sip from a health potion");
		actiontextbuilder_create_text("+1", C_GREEN, &player->sprite->pos);
	} else {
		gui_log("You have nothing to sip");
	}
}

	static Vector2d
read_direction_from(Input *input)
{
	if (input_key_is_pressed(input, KEY_LEFT))
		return VECTOR2D_LEFT;
	else if (input_key_is_pressed(input, KEY_RIGHT))
		return VECTOR2D_RIGHT;
	else if (input_key_is_pressed(input, KEY_UP))
		return VECTOR2D_UP;
	else if (input_key_is_pressed(input, KEY_DOWN))
		return VECTOR2D_DOWN;
	else
		return VECTOR2D_NODIR;
}

	static void
handle_next_move(UpdateData *data)
{
	static unsigned int step = 1;

	Player *player = data->player;
	Position originPos = player->sprite->pos;

	// Don't move when projectiles are still moving
	if (linkedlist_size(player->projectiles) > 0)
		return;

	if (input_key_is_pressed(data->input, KEY_SPACE)) {
		action_spent(data->player);
		gui_log("You take a moment and stare at your feet");
		return;
	}

	RoomMatrix *matrix = data->matrix;
	Vector2d nextDir = read_direction_from(data->input);
	if (!vector2d_equals(nextDir, VECTOR2D_NODIR))
		move(player, matrix, nextDir);

	if (!position_equals(&originPos, &player->sprite->pos)
	    && matrix->modifier->type == RMOD_TYPE_FIRE) {
		Object *o = object_create_fire();
		o->damage *= player->stats.lvl;
		o->sprite->pos = originPos;
		linkedlist_append(&data->map->objects, o);
	}

	map_room_modifier_player_effect(player, matrix, &nextDir, move);

	if (!vector2d_equals(VECTOR2D_NODIR, nextDir)) {
		player->sprite->clip.x = 16*step;
		++step;
		step = step % 4;
	}
}

	static void
use_skill(Skill *skill, SkillData *skillData)
{
	skill->active = false;
	skill->use(skill, skillData);
	if (skill->actionRequired)
		action_spent(skillData->player);
	skill->resetCountdown = skill->resetTime;
}

	static void
check_skill_activation(UpdateData *data)
{
	// TODO(Linus): This could be "smarter"
	Player *player = data->player;
	Input *input = data->input;
	RoomMatrix *matrix = data->matrix;

	unsigned int selected = 0;
	if (input_key_is_pressed(input, KEY_NUM1)) {
		selected = 1;
	} else if (input_key_is_pressed(input, KEY_NUM2)) {
		selected = 2;
	} else if (input_key_is_pressed(input, KEY_NUM3)) {
		selected = 3;
	} else if (input_key_is_pressed(input, KEY_NUM4)) {
		selected = 4;
	} else if (input_key_is_pressed(input, KEY_NUM5)) {
		selected = 5;
	}

	if (selected == 0)
		return;

	for (size_t i = 0; i < PLAYER_SKILL_COUNT; ++i) {
		if (!player->skills[i])
			continue;

		Skill *skill = player->skills[i];
		if (skill->levelcap > player->stats.lvl)
			continue;
		if (skill->available && !skill->available(player))
			continue;
		skill->active = (selected - 1) == i && !skill->active && skill->resetCountdown == 0;
		if (skill->active && skill->instantUse) {
			SkillData skillData = { player, matrix, VECTOR2D_NODIR };
			use_skill(skill, &skillData);
		}
	}
}

	static bool
check_skill_trigger(UpdateData *data)
{
	Player *player = data->player;
	RoomMatrix *matrix = data->matrix;
	Input *input = data->input;

	int activeSkill = -1;
	for (int i = 0; i < PLAYER_SKILL_COUNT; ++i) {
		if (player->skills[i] && player->skills[i]->active) {
			activeSkill = i;
			break;
		}
	}

	if (activeSkill < 0)
		return false;

	Vector2d nextDir = read_direction_from(input);
	if (vector2d_equals(nextDir, VECTOR2D_NODIR))
		return false;

	SkillData skillData = { player, matrix, nextDir };
	use_skill(player->skills[activeSkill], &skillData);

	return true;
}

	static void
build_sword_animation(Player *p, SDL_Renderer *renderer)
{
	animation_load_texture(p->swordAnimation, "Extras/SwordSwing.png", renderer);
	animation_set_frames(p->swordAnimation, (AnimationClip[]) {
			     {  0, 0, 16, 16, 20 },
			     { 16, 0, 16, 16, 20 },
			     { 32, 0, 16, 16, 20 },
			     { 48, 0, 16, 16, 20 },
			     { 64, 0, 16, 16, 20 }
			     });

	p->swordAnimation->loop = false;
	p->swordAnimation->sprite->dim = GAME_DIMENSION;
	p->swordAnimation->sprite->clip = (SDL_Rect) { 0, 0, 16, 16 };
	p->swordAnimation->sprite->rotationPoint = (SDL_Point) { 16, 16 };
}

	Player* 
player_create(class_t class, Camera *cam)
{
	Player *player = malloc(sizeof(Player));
	player->sprite = sprite_create();
#ifdef DEBUG
	player->daggers			= 10;
	player->gold			= 500;
#else
	player->daggers			= 0;
#endif
	player->stat_data.total_steps	= 0;
	player->stat_data.steps		= 0;
	player->stat_data.hits		= 0;
	player->stat_data.kills		= 0;
	player->stat_data.misses	= 0;
	player->xp			= 0;
	player->gold			= 0;
	player->potion_sips		= 0;
	player->phase_count		= 0;
	player->class			= class;
	player->state			= ALIVE;
	player->projectiles		= linkedlist_create();
	player->animationTimer		= _timer_create();
	player->swordAnimation		= animation_create(5);
	player->equipment.hasArtifacts	= false;
	player->equipment.keys		= 0;
	player->stateData.shopOwnerKiller = false;

	build_sword_animation(player, cam->renderer);

	memset(&player->skills,
	       0, PLAYER_SKILL_COUNT * sizeof(Skill*));

	for (size_t i = 0; i < LAST_ARTIFACT_EFFECT; ++i)
		player->equipment.artifacts[i].level = 0;

	char asset[100];
	switch (class) {
		case ENGINEER:
			m_strcpy(asset, 100, "Commissions/Engineer.png");
			player->stats = (Stats) ENGINEER_STATS;
			break;
		case MAGE:
			m_strcpy(asset, 100, "Commissions/Mage.png");
			player->stats = (Stats) MAGE_STATS;
			player->skills[0] = skill_create(VAMPIRIC_BLOW, cam);
			player->skills[1] = skill_create(ERUPT, cam);
			player->skills[2] = skill_create(BLINK, cam);
			break;
		case PALADIN:
			m_strcpy(asset, 100, "Commissions/Paladin.png");
			player->stats = (Stats) PALADIN_STATS;
			break;
		case ROGUE:
			m_strcpy(asset, 100, "Commissions/Rogue.png");
			player->stats = (Stats) ROGUE_STATS;
			player->skills[0] = skill_create(BACKSTAB, cam);
			player->skills[1] = skill_create(TRIP, cam);
			player->skills[2] = skill_create(PHASE, cam);
			player->daggers = 10;
			break;
		case WARRIOR:
			m_strcpy(asset, 100, "Commissions/Warrior.png");
			player->stats = (Stats) WARRIOR_STATS;
			player->skills[0] = skill_create(FLURRY, cam);
			player->skills[1] = skill_create(BASH, cam);
			player->skills[2] = skill_create(CHARGE, cam);
			break;
	}

	player->skills[3] = skill_create(DAGGER_THROW, cam);
	player->skills[4] = skill_create(SIP_HEALTH, cam);

	sprite_load_texture(player->sprite, asset, 0, cam->renderer);
	player->sprite->pos = (Position) { TILE_DIMENSION, TILE_DIMENSION };
	player->sprite->dim = GAME_DIMENSION;
	player->sprite->clip = (SDL_Rect) { 0, 0, 16, 16 };

	return player;
}

	void
player_reset_on_levelchange(Player *player)
{
	player->sprite->pos = (Position) {
		TILE_DIMENSION, TILE_DIMENSION };
	player->equipment.keys = 0;
}

ExperienceData player_get_xp_data(Player *p)
{
	ExperienceData data;
	data.previousLevel = next_level_threshold(p->stats.lvl - 1);
	data.current = p->xp;
	data.nextLevel = next_level_threshold(p->stats.lvl);
	data.level = p->stats.lvl;
	return data;
}

	void
player_monster_kill_check(Player *player, Monster *monster)
{
	if (!monster)
		return;

	if (monster->stats.hp <= 0) {
		unsigned int gained_xp = 5 * monster->stats.lvl;
		player->stat_data.kills += 1;
		mixer_play_effect(DEATH);
		gui_log("You killed %s and gained %d xp",
			monster->lclabel, gained_xp);
		player_gain_xp(player, gained_xp);

#ifdef STEAM_BUILD
		if (strcmp("The Shadow", monster->label) == 0)
			steam_set_achievement(LIGHTS_ON);
		else if (strcmp("The Hell Hound", monster->label) == 0)
			steam_set_achievement(BAD_DOG);
		else if (strcmp("Platino", monster->label) == 0)
			steam_set_achievement(DRAGON_SLAYER);
		else if (strcmp("The Cleric", monster->label) == 0)
			steam_set_achievement(THE_DOCTOR_IS_OUT);
		else if (strcmp("Linus, the Developer", monster->label) == 0)
			steam_set_achievement(BUGGFIXER);
		else if (strcmp("Ti, the Mage", monster->label) == 0)
			steam_set_achievement(BUGGCREATOR);
#endif // STEAM_BUILD
		if (strcmp("The Trader", monster->label) == 0) {
			player->stateData.shopOwnerKiller = true;
		}

		Event event;
		event.type = MONSTER_KILLED_EVENT;
		event.monsterKilled.monster = monster;
		event.monsterKilled.player = player;
		event_trigger(&event);
	}
}

	void
player_hit(Player *p, unsigned int dmg)
{
	if (dmg > 0) {
		Position pos = p->sprite->pos;
		pos.x += 8;
		pos.y += 8;
		particle_engine_bloodspray(pos, DIM(8, 8), dmg);
		mixer_play_effect(PLAYER_HIT0 + get_random(2));
		char msg[5];
		m_sprintf(msg, 5, "-%d", dmg);
		actiontextbuilder_create_text(msg,
					      C_RED,
					      &p->sprite->pos);

		gamecontroller_rumble(0.50, 100);
	} else {
		actiontextbuilder_create_text("Dodged",
					      C_YELLOW,
					      &p->sprite->pos);
	}

}

	void
player_render(Player *player, Camera *cam)
{
	sprite_set_alpha(player->sprite, player->phase_count ? 150 : 255);
	sprite_render(player->sprite, cam);

	LinkedList *projectile = player->projectiles;
	while (projectile) {
		projectile_render(projectile->data, cam);
		projectile = projectile->next;
	}
}

	void
player_render_toplayer(Player *player, Camera *camera)
{
	animation_render(player->swordAnimation, camera);
}

	void
player_reset_steps(Player *p)
{
	p->stat_data.steps = 0;
}

	static void
reset_dagger_skill(Player *p)
{
	p->skills[3]->resetCountdown = 0;
}

	void
player_update(UpdateData *data)
{
	Player *player = data->player;
	if (player->stats.hp <= 0)
		return;

	sprite_update(player->sprite, data);

	check_skill_activation(data);
	if (player->sprite->state != SPRITE_STATE_FALLING && !check_skill_trigger(data))
		handle_next_move(data);

	if (player->sprite->state == SPRITE_STATE_FALLING && player->stats.hp > 0) {
		if (player->sprite->dim.width <= 4) {
			player->stats.hp = 0;
			player->sprite->state = SPRITE_STATE_DEFAULT;
		}
	}

	LinkedList *remaining = linkedlist_create();
	while (player->projectiles) {
		Projectile *p = linkedlist_pop(&player->projectiles);
		projectile_update(p, data);
		if (p->alive) {
			linkedlist_push(&remaining, p);
		} else {
			projectile_destroy(p);
			action_spent(player);
			if (player->class == ROGUE)
				reset_dagger_skill(player);
		}
	}

	linkedlist_destroy(&player->projectiles);
	player->projectiles = remaining;

	animation_update(player->swordAnimation);
}

	static void
player_reset(Player *player)
{
	for (size_t i = 0; i < LAST_ARTIFACT_EFFECT; ++i)
		player->equipment.artifacts[i].level = 0;
	player->equipment.hasArtifacts = false;

	while (player->projectiles)
		projectile_destroy(linkedlist_pop(&player->projectiles));
}

	void
player_destroy(Player *player)
{
	player_reset(player);

	if (player->sprite)
		sprite_destroy(player->sprite);

	animation_destroy(player->swordAnimation);
	timer_destroy(player->animationTimer);

	for (size_t i = 0; i < PLAYER_SKILL_COUNT; ++i) {
		if (player->skills[i])
			skill_destroy(player->skills[i]);
		player->skills[i] = NULL;
	}

	free(player);
}

	bool
player_turn_over(Player *player)
{
	return player->stat_data.steps >= player->stats.speed;
}

	Uint32
player_has_artifact(Player *p, MagicalEffect effect)
{
	return p->equipment.artifacts[effect].level;
}

	void
player_add_artifact(Player *p, Artifact *a)
{
	if (a->price > p->gold) {
		gui_log("You don't have enough gold to buy a %s",
			a->info.name);
		return;
	}

	if (a->collected)
		return;

	if (a->price) {
		gui_log("You pay %d gold for %s", a->price, a->info.name);
		p->gold -= a->price;
	}

	mixer_play_effect(MAGIC_PICKUP);
	a->collected = true;

	ArtifactData *ad = &p->equipment.artifacts[a->effect];
	ad->name = a->info.name;
	ad->desc = a->info.desc;
	ad->level += a->level;

	gui_log("You pick an ancient %s", ad->name);
	gui_log("%s (%u)", ad->desc, ad->level);
	p->equipment.hasArtifacts = true;
}

	void
player_set_falling(Player *player)
{
	mixer_play_effect(FALL0 + get_random(1));
	player->sprite->clip = CLIP16(0, 0);
	player->sprite->state = SPRITE_STATE_FALLING;
}
