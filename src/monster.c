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
#include <assert.h>

#include "monster.h"
#include "util.h"
#include "player.h"
#include "monster.h"
#include "random.h"
#include "gui.h"
#include "item.h"
#include "item_builder.h"
#include "map.h"
#include "particle_engine.h"
#include "defines.h"
#include "update_data.h"
#include "actiontextbuilder.h"
#include "texturecache.h"
#include "trap.h"
#include "object.h"
#include "mixer.h"

static void
monster_set_sprite_clip_for_current_state(Monster *m)
{
	switch (m->state.current) {
		case AGRESSIVE:
			m->stateIndicator.sprite->clip = CLIP16(16 * 11, 16 * 3);
			break;
		case SCARED:
			m->stateIndicator.sprite->clip = CLIP16(16 * 12, 16 * 3);
			break;
		case SLEEPING:
			m->stateIndicator.sprite->clip = CLIP16(16 * 10, 16 * 4);
			break;
		case SCANNING:
			m->stateIndicator.sprite->clip = CLIP16(16 * 13, 16 * 4);
		default:
			break;
	}
}

static void
monster_set_state_display_time(Monster *m)
{
	switch (m->state.current) {
		case SLEEPING:
			m->stateIndicator.displayCount = -1;
			break;
		case PASSIVE:
		case STATIONARY:
		case STUNNED:
			m->stateIndicator.displayCount = 0;
			break;
		default:
			m->stateIndicator.displayCount = 5;
	}
}

static void
monster_state_change(Monster *m, StateType newState)
{
	if (m->state.current == newState)
		return;

	m->state.last = m->state.current;
	m->state.current = newState;
	m->state.stepsSinceChange = 0;
	m->state.forceCount = 0;

	monster_set_state_display_time(m);

	if (newState == STUNNED) {
		m->stats.disadvantage = true;
	} else if (m->state.last == STUNNED) {
		m->stats.disadvantage = false;
		m->sprite->angle = 0;
	}

	monster_set_sprite_clip_for_current_state(m);
}

static void
monster_behaviour_check_post_hit(Monster *m)
{
	if (m->state.stepsSinceChange < m->state.forceCount)
		return;

	switch (m->behaviour) {
		case PACIFIST:
		case COWARD:
			monster_state_change(m, SCARED);
			break;
		case GUERILLA:
		case ASSASSIN:
		case SORCERER:
		case FIRE_DEMON:
			break;
		default:
			monster_state_change(m, AGRESSIVE);
			break;
	}
}

static void
damage_surroundings(Monster *m, RoomMatrix *rm)
{
	Position roomPos = position_to_matrix_coords(&m->sprite->pos);
	for (Sint32 i = -1; i <= 1; ++i) {
		for (Sint32 j = -1; j <= 1; ++j) {
			if (i == 0 && j == 0)
				continue;
			RoomSpace *r = &rm->spaces[roomPos.x + i][roomPos.y + j];
			if (r->monster) {
				int dmg = stats_fight(&m->stats, &r->monster->stats);
				monster_hit(r->monster, dmg);
				gui_log("%s takes %d damage from the explosion", r->monster->label, dmg);
			} else if (r->player) {
				int dmg = stats_fight(&m->stats, &r->player->stats);
				player_hit(r->player, dmg);
				gui_log("You take %d damage from the explosion", dmg);
			}
		}
	}
}

static void
sorcerer_blast(Monster *m, RoomMatrix *rm)
{
	gui_log("%s creates a magical explosion", m->label);
	particle_engine_eldritch_explosion(m->sprite->pos, DIM(TILE_DIMENSION, TILE_DIMENSION));
	mixer_play_effect(BURST);

	damage_surroundings(m, rm);
}

static void
assassin_cloak_effect(Monster *m, bool cloak)
{
	if (cloak)
		gui_log("%s dissappears from sight", m->label);
	else
		gui_log("%s reappears, filled with rage", m->label);
	particle_engine_fire_explosion(m->sprite->pos, DIM(TILE_DIMENSION, TILE_DIMENSION));
	m->sprite->hidden = cloak;
	m->stateIndicator.sprite->hidden = cloak;
	mixer_play_effect(cloak ? FADE_OUT : FADE_IN);
}


static void
monster_behaviour_check_post_attack(Monster *m)
{
	switch (m->behaviour) {
		case GUERILLA:
		case SORCERER:
		case FIRE_DEMON:
			monster_state_change(m, SCARED);
			break;
		case ASSASSIN:
			assassin_cloak_effect(m, true);
			monster_state_change(m, SCARED);
			break;
		default:
			break;
	}
}

static void
handle_sentinel_behaviour(Monster *m, RoomMatrix *rm)
{
	if (m->state.current == AGRESSIVE)
		return;

	Position monsterPos = m->sprite->pos;
	if (!position_in_room(&monsterPos, &rm->roomPos))
		return;

	monsterPos = position_to_matrix_coords(&monsterPos);
	Position playerPos = rm->playerRoomPos;
	unsigned int dx = abs(playerPos.x - monsterPos.x);
	unsigned int dy = abs(playerPos.y - monsterPos.y);
	if (dx < 3 && dy < 3)
		monster_state_change(m, AGRESSIVE);
	else if (dx <= 3 && dy <= 3)
		monster_state_change(m, SCANNING);
	else
		monster_state_change(m, SLEEPING);
}

static void
monster_behaviour_check(Monster *m, RoomMatrix *rm)
{
	switch (m->behaviour) {
		case GUERILLA:
		case SORCERER:
		case FIRE_DEMON:
			if (m->state.stepsSinceChange > 5
			    && m->state.current == SCARED) {
				monster_state_change(m, AGRESSIVE);
			}
			break;
		case ASSASSIN:
			if (m->state.stepsSinceChange > 5
			    && m->state.current == SCARED) {
				assassin_cloak_effect(m, false);
				monster_state_change(m, AGRESSIVE);
			}
			break;
		case SENTINEL:
			handle_sentinel_behaviour(m, rm);
			break;
		default:
			break;
	}
}

Monster*
monster_create(void)
{
	Monster *m = ec_malloc(sizeof(Monster));
	m->sprite = sprite_create();
	m->sprite->dim = GAME_DIMENSION;
	m->sprite->clip = (SDL_Rect) { 0, 0, 16, 16 };
	m->sprite->rotationPoint = (SDL_Point) { 16, 16 };

	m->stats = (Stats) {
		12, // Max HP
		12, // hp
		2,  // dmg
		0,  // atk
		0,  // def
		1,  // speed
		1,   // lvl
		false, // advantage
		false // disadvantage
	};

	m->label = NULL;
	m->lclabel = NULL;
	m->steps = 0;
	m->stateIndicator.sprite = sprite_create();
	sprite_set_texture(m->stateIndicator.sprite, texturecache_add("GUI/GUI0.png"), 0);
	sprite_set_texture(m->stateIndicator.sprite, texturecache_add("GUI/GUI1.png"), 1);
	m->stateIndicator.sprite->dim = GAME_DIMENSION;
	m->stateIndicator.displayCount = 0;
	m->stateIndicator.shownOnPlayerRoomEnter = false;
	m->state.forceCount = 0;
	m->boss = false;
	monster_set_behaviour(m, NORMAL);

	return m;
}

void
monster_update_pos(Monster *m, Position pos)
{
	m->sprite->pos = pos;

	Position indicatorPos = pos;
	indicatorPos.y -= 32;
	m->stateIndicator.sprite->pos = indicatorPos;
}

static bool
has_collided(Monster *monster, RoomMatrix *matrix, Vector2d direction)
{
	if (!position_in_room(&monster->sprite->pos, &matrix->roomPos))
		return true;

	RoomSpace *space = roommatrix_get_space_for(matrix, &monster->sprite->pos);

	if (space->player && monster->state.current == AGRESSIVE) {
		unsigned int dmg = stats_fight(&monster->stats,
					       &space->player->stats);

		player_hit(space->player, dmg);

		if (dmg > 0) {
			gui_log("%s hit you for %u damage",
				monster->label, dmg);
			camera_shake(direction, 300);
		} else {
			gui_log("%s missed you", monster->label);
		}
		monster_behaviour_check_post_attack(monster);
	}

	return space->occupied || space->monster;
}

static bool
move(Monster *m, RoomMatrix *rm, Vector2d direction)
{
	m->sprite->pos.x += TILE_DIMENSION * (int) direction.x;
	m->sprite->pos.y += TILE_DIMENSION * (int) direction.y;

	RoomSpace *space = roommatrix_get_space_for(rm, &m->sprite->pos);
	if (has_collided(m, rm, direction) || space->lethal || space->trap || space->damaging) {
		m->sprite->pos.x -= TILE_DIMENSION * (int) direction.x;
		m->sprite->pos.y -= TILE_DIMENSION * (int) direction.y;
		return false;
	}
	return true;
}

static void
monster_drunk_walk(Monster *m, RoomMatrix *rm)
{
	unsigned int i, maxMoveAttempts = 6;

	if (get_random(5) >= 2)
		return;

	for (i = 0; i < maxMoveAttempts; ++i) {
		int moveDir = get_random(3);
		if (moveDir == 0 && move(m, rm, VECTOR2D_LEFT))
			break;
		else if (moveDir == 1 && move(m, rm, VECTOR2D_RIGHT))
			break;
		else if (moveDir == 2 && move(m, rm, VECTOR2D_UP))
			break;
		else if (moveDir == 3 && move(m, rm, VECTOR2D_DOWN))
			break;
	}
}

static Direction
get_optimal_move_towards(Monster *m, RoomMatrix *rm, const Position *dest)
{
	int x_dist, y_dist;
	Position mPos;

	mPos = position_to_matrix_coords(&m->sprite->pos);

	unsigned int currentScore = 100;
	unsigned int chosenDirection = UP;

	for (unsigned int i = 0; i < 4; ++i) {
		Position next = mPos;
		unsigned int nextScore = 0;

		switch (i) {
			case UP:
				next.y -= 1;
				break;
			case DOWN:
				next.y += 1;
				break;
			case LEFT:
				next.x -= 1;
				break;
			case RIGHT:
				next.x += 1;
				break;
		}

		if (position_equals(&next, dest)) {
			chosenDirection = (Direction) i;
			break;
		}

		if (!position_in_roommatrix(&next))
			continue;

		x_dist = abs(next.x - dest->x);
		y_dist = abs(next.y - dest->y);

		if (rm->spaces[next.x][next.y].occupied
		    || rm->spaces[next.x][next.y].lethal
		    || rm->spaces[next.x][next.y].trap) {
			nextScore += 50;
		}

		nextScore += x_dist > y_dist ? x_dist : y_dist;
		if (nextScore < currentScore) {
			currentScore = nextScore;
			chosenDirection = (Direction) i;
		}
	}

	return chosenDirection;
}

static void
monster_agressive_walk(Monster *m, RoomMatrix *rm)
{
	unsigned int chosenDirection = get_optimal_move_towards(m, rm, &rm->playerRoomPos);

	switch (chosenDirection) {
		case UP:
			move(m, rm, VECTOR2D_UP);
			break;
		case DOWN:
			move(m, rm, VECTOR2D_DOWN);
			break;
		case LEFT:
			move(m, rm, VECTOR2D_LEFT);
			break;
		case RIGHT:
			move(m, rm, VECTOR2D_RIGHT);
			break;
	}
}

static void
monster_coward_walk(Monster *m, RoomMatrix *rm)
{
	int x_dist, y_dist;
	Position mPos;

	mPos = position_to_matrix_coords(&m->sprite->pos);

	x_dist = mPos.x - rm->playerRoomPos.x;
	y_dist = mPos.y - rm->playerRoomPos.y;

	if (abs(x_dist) > abs(y_dist)) {
		if (x_dist > 0)
			move(m, rm, VECTOR2D_RIGHT);
		else
			move(m, rm, VECTOR2D_LEFT);
	} else {
		if (y_dist > 0)
			move(m, rm, VECTOR2D_DOWN);
		else
			move(m, rm, VECTOR2D_UP);
	}
}

static void
on_monster_move(Monster *m, Position *origPos, Map *map, RoomMatrix *rm)
{
	Position currentTilePos = position_to_matrix_coords(&m->sprite->pos); 
	Player *player = rm->spaces[rm->playerRoomPos.x][rm->playerRoomPos.y].player;
	if (player) {
		Uint32 range = 3 + player_has_artifact(player, IMPROVED_HEARING) * 2;
		bool withinHearingDist =
			range > 3 && position_proximity(range,
							&currentTilePos,
							&rm->playerRoomPos);

		RoomSpace *space = &rm->spaces[currentTilePos.x][currentTilePos.y];
		if (space->light < 100 && withinHearingDist) {
			Position alertPos = m->sprite->pos;
			alertPos.x += TILE_DIMENSION >> 1;
			alertPos.y += TILE_DIMENSION >> 1;
			actiontextbuilder_create_text("!", C_WHITE, &alertPos);
		}
	}

	if (rm->modifier->type == RMOD_TYPE_FIRE || m->behaviour == FIRE_DEMON) {
		Object *o = object_create_fire();
		o->sprite->pos = *origPos;
		o->damage *= m->stats.lvl;
		linkedlist_push(&map->objects, o);
	}
	if (m->behaviour == SORCERER) {
		Object *o = object_create_green_gas();
		o->sprite->pos = *origPos;
		o->damage *= m->stats.lvl;
		linkedlist_push(&map->objects, o);
	}
}

static void
monster_walk(Monster *m, RoomMatrix *rm)
{
	switch (m->state.current) {
		case PASSIVE:
			monster_drunk_walk(m, rm);
			break;
		case AGRESSIVE:
			monster_agressive_walk(m, rm);
			break;
		case SCARED:
			monster_coward_walk(m, rm);
			break;
		case STATIONARY:
		case SLEEPING:
		case SCANNING:
		default:
			break;
	};

	monster_update_pos(m, m->sprite->pos);
}

static bool
monster_check_steps(Monster *m)
{
	m->steps++;
	if (m->steps >= m->stats.speed) {
		if (m->stateIndicator.displayCount > 0)
			m->stateIndicator.displayCount -= 1;
		m->state.stepsSinceChange += 1;
		return true;
	}

	return false;
}

static bool
monster_perform_aoe_attack(Monster *m, RoomMatrix *rm)
{
	if (m->state.current == AGRESSIVE && m->behaviour == SORCERER) {
		Position tilePos = position_to_matrix_coords(&m->sprite->pos);
		if (position_proximity(1, &tilePos, &rm->playerRoomPos)) {
			sorcerer_blast(m, rm);
			monster_behaviour_check_post_attack(m);
			return true;
		}
	}
	return false;
}

bool
monster_move(Monster *m, RoomMatrix *rm, Map *map)
{
	Player *player = roommatrix_get_player(rm);
	if (player && player->phase_count)
		return true;

	if (m->stats.hp <= 0 || m->sprite->state == SPRITE_STATE_FALLING)
		return true;

	if (m->state.forceCount) {
		if (m->state.stepsSinceChange >= m->state.forceCount) {
			monster_state_change(m, m->state.last);
			monster_behaviour_check_post_hit(m);
		}
	}

	if (m->state.current == STUNNED) {
		m->state.stepsSinceChange += 1;
		return true;
	}

	monster_behaviour_check(m, rm);
	Position origPos = m->sprite->pos;

	if (monster_perform_aoe_attack(m, rm)) {
		return true;
	}

	Position originalMPos = position_to_matrix_coords(&m->sprite->pos);

	rm->spaces[originalMPos.x][originalMPos.y].occupied = false;
	rm->spaces[originalMPos.x][originalMPos.y].monster = NULL;

	monster_walk(m, rm);

	Position newPos = position_to_matrix_coords(&m->sprite->pos);
	rm->spaces[newPos.x][newPos.y].occupied = true;
	rm->spaces[newPos.x][newPos.y].monster = m;

	if (!position_equals(&originalMPos, &newPos)) {
		on_monster_move(m, &origPos, map, rm);
	}

	return monster_check_steps(m);
}

void
monster_reset_steps(Monster *m)
{
	m->steps = 0;
}

void
monster_update(Monster *m, UpdateData *data)
{
	if (m->stats.hp <= 0)
		return;

	sprite_update(m->sprite, data);

	if (m->sprite->state == SPRITE_STATE_PLUMMETED) {
		m->stats.hp = 0;
		player_monster_kill_check(data->player, m);
		return;
	}

	Position monsterRoomPos = position_to_room_coords(&m->sprite->pos);
	if (position_equals(&data->matrix->roomPos, &monsterRoomPos)) {

		if (!m->stateIndicator.shownOnPlayerRoomEnter) {
			m->stateIndicator.shownOnPlayerRoomEnter = true;
			monster_set_state_display_time(m);
		} else {
			m->stateIndicator.shownOnPlayerRoomEnter = false;
		}

		if (m->sprite->state != SPRITE_STATE_FALLING &&
		    m->sprite->state != SPRITE_STATE_PLUMMETED) {
			RoomSpace *space = roommatrix_get_space_for(data->matrix, &m->sprite->pos);
			if (space && space->lethal) {
				m->sprite->state = SPRITE_STATE_FALLING;
			}
		}
	}

	if (m->state.current == STUNNED) {
		Sprite *s = m->sprite;
		if (s->angle > 30)
			s->angle = -1;
		else if (s->angle < -30)
			s->angle = 0;

		if (s->angle >= 0)
			s->angle += 2;
		else
			s->angle -= 2;
	}
}

void
monster_hit(Monster *monster, unsigned int dmg)
{
	if (dmg > 0) {
		Position p = monster->sprite->pos;
		p.x += 8;
		p.y += 8;
		Dimension d = { 8, 8 };
		particle_engine_bloodspray(p, d, dmg);
		char msg[10];
		m_sprintf(msg, 10, "-%d", dmg);
		actiontextbuilder_create_text(msg,
					      C_RED,
					      &monster->sprite->pos);
	} else {
		actiontextbuilder_create_text("Dodged",
					      C_YELLOW,
					      &monster->sprite->pos);
	}
	monster_behaviour_check_post_hit(monster);
}

void
monster_update_stats_for_level(Monster *m, unsigned int level)
{
	m->stats.lvl = level;
	m->stats.maxhp *= level;
	m->stats.hp = m->stats.maxhp;
	m->stats.dmg *= level;
	m->stats.atk *= level;
	m->stats.def *= level;
}

void
monster_drop_loot(Monster *monster, Map *map, Player *player)
{
	static unsigned int treasure_drop_chance = 1;
	unsigned int item_drop_chance = 1;

	if (monster->sprite->state == SPRITE_STATE_FALLING)
		return;

	Item *item;
	Item *items[3];
	unsigned int item_count = 0;
	bool player_full_health = player->stats.hp >= player->stats.maxhp;

	Position monsterTilePos = monster->sprite->pos;
	monsterTilePos.x -= (monsterTilePos.x % TILE_DIMENSION);
	monsterTilePos.y -= (monsterTilePos.y % TILE_DIMENSION);

	if (monster->boss) {
		Artifact *a = artifact_create_random(player, 2);
		a->sprite->pos = monsterTilePos;
		linkedlist_append(&map->artifacts, a);
		Item *treasure = item_builder_build_item(TREASURE, map->level*2);
		treasure->sprite->pos = monsterTilePos;
		linkedlist_append(&map->items, treasure);
	}

	if (strcmp(monster->label, "The Trader") == 0) {
		Item *treasure = item_builder_build_treasure(PLATINUM, 25 * monster->stats.lvl);
		treasure->sprite->pos = monsterTilePos;
		linkedlist_append(&map->items, treasure);
	}

	if (monster->stats.lvl > 2 && get_random(29) == 0) {
		Artifact *a = artifact_create_random(player, 1);
		a->sprite->pos = monsterTilePos;
		linkedlist_append(&map->artifacts, a);
	}

	if (player->stats.hp < player->stats.maxhp / 2)
		item_drop_chance = 0;

	if (get_random(treasure_drop_chance) == 0) {
		item = item_builder_build_item(TREASURE, map->level);
		item->sprite->pos = monsterTilePos;
		items[item_count++] = item;
	}
	if (get_random(item_drop_chance) == 0) {
		ItemKey key;
		key = get_random(DAGGER);
		item = item_builder_build_item(key, map->level);
		item->sprite->pos = monsterTilePos;
		items[item_count++] = item;
	}
	if (!player_full_health && get_random(2) == 0) {
		item = item_builder_build_item(FLESH, map->level);
		item->sprite->pos = monsterTilePos;
		items[item_count++] = item;
	}

	if (item_count == 0)
		return;

	gui_log("%s dropped something", monster->label);

	if (item_count == 1) {
		linkedlist_append(&map->items, items[0]);
	} else {
		Item *container = item_builder_build_sack();
		container->sprite->pos = monsterTilePos;
		unsigned int i;
		for (i = 0; i < item_count; ++i) {
			linkedlist_append(&container->items, items[i]);
		}
		linkedlist_append(&map->items, container);
	}
}

void
monster_render(Monster *m, Camera *cam)
{
	if (m->stats.hp <= 0)
		return;

	sprite_render(m->sprite, cam);
}

void
monster_render_top_layer(Monster *m, RoomMatrix *rm, Camera *cam)
{
	if (m->stats.hp <= 0)
		return;

	Position mPos = position_to_matrix_coords(&m->sprite->pos);
	mPos.y -= 1;
	if (rm->spaces[mPos.x][mPos.y].player) {
		sprite_set_alpha(m->stateIndicator.sprite, 110);
	}
	if (m->stateIndicator.displayCount != 0)
		sprite_render(m->stateIndicator.sprite, cam);
	if (rm->spaces[mPos.x][mPos.y].player) {
		sprite_set_alpha(m->stateIndicator.sprite, 255);
	}
}

void
monster_set_behaviour(Monster *m, MonsterBehaviour behaviour)
{
	m->behaviour = behaviour;
	switch (behaviour) {
		case HOSTILE:
		case GUERILLA:
		case ASSASSIN:
		case SORCERER:
		case COWARD:
		case FIRE_DEMON:
			m->state.current = AGRESSIVE;
			break;
		case SENTINEL:
			m->state.current = SLEEPING;
			break;
		case PACIFIST:
		case NORMAL:
		default:
			m->state.current = PASSIVE;
			break;
	}
	monster_set_sprite_clip_for_current_state(m);
}

void
monster_set_state(Monster *m, StateType state, Uint8 forceCount)
{
	monster_state_change(m, state);
	m->state.forceCount = forceCount;
}

void
monster_push(Monster *m, Player *p, RoomMatrix *rm, Vector2d direction)
{
	m->sprite->pos.x += TILE_DIMENSION * (int) direction.x;
	m->sprite->pos.y += TILE_DIMENSION * (int) direction.y;

	RoomSpace *space = roommatrix_get_space_for(rm, &m->sprite->pos);
	if (space->trap) {
		int dmg = space->trap->damage * 3;
		m->stats.hp -= dmg;
		monster_hit(m, dmg);
		gui_log("%s takes %d damage from a trap", m->label, dmg);
	} else if (space->damaging) {
		LinkedList *objects = space->objects;
		while (objects) {
			Object *o = objects->data;
			objects = objects->next;
			if (!o->damage)
				return;
			m->stats.hp -= o->damage * 3;
			monster_hit(m, o->damage * 3);
		}
	} else if (has_collided(m, rm, direction)) {
		m->sprite->pos.x -= TILE_DIMENSION * (int) direction.x;
		m->sprite->pos.y -= TILE_DIMENSION * (int) direction.y;
	}

	monster_update_pos(m, m->sprite->pos);
	player_monster_kill_check(p, m);
}

void
monster_destroy(Monster *m)
{
	sprite_destroy(m->sprite);
	if (m->label)
		free(m->label);
	if (m->lclabel)
		free(m->lclabel);
	sprite_destroy(m->stateIndicator.sprite);
	free(m);
}
