#include <assert.h>
#include "monster.h"
#include "util.h"
#include "player.h"
#include "monster.h"
#include "random.h"

static void
monster_load_texts(Monster *m, SDL_Renderer *renderer)
{
	ActionText *t = actiontext_create();
	actiontext_load_font(t, "assets/GUI/SDS_6x6.ttf", 14);
	t->color = (SDL_Color) { 255, 100, 0 };
	actiontext_set_text(t, "HIT", renderer);
	t->pos = m->sprite->pos;
	m->hitText = t;

	t = actiontext_create();
	actiontext_load_font(t, "assets/GUI/SDS_6x6.ttf", 14);
	t->color = (SDL_Color) { 255, 255, 0 };
	actiontext_set_text(t, "MISS", renderer);
	t->pos = m->sprite->pos;
	m->missText = t;
}

Monster*
monster_create(SDL_Renderer *renderer)
{
	Monster *m = ec_malloc(sizeof(Monster));
	m->sprite = sprite_create();
	m->sprite->clip = (SDL_Rect) { 0, 0, 16, 16 };
	m->stats = (Stats) { 11, 1, 0, 0, 1, 1 };
	m->state.normal = PASSIVE;
	m->state.challenge = AGRESSIVE;
	m->state.current = m->state.normal;

	monster_load_texts(m, renderer);

	return m;
}

void
monster_update_pos(Monster *m, Position pos)
{
	m->sprite->pos = pos;

	Position textPos = pos;
	textPos.y += 10;
	m->hitText->pos = textPos;
	m->missText->pos = textPos;
}

static bool
has_collided(Monster *monster, RoomMatrix *matrix)
{
	if (!position_in_room(&monster->sprite->pos, &matrix->roomPos))
		return true;

	Position roomPos = position_to_matrix_coords(&monster->sprite->pos);
	RoomSpace *space = &matrix->spaces[roomPos.x][roomPos.y];

	if (space->player) {
		unsigned int dmg = stats_fight(&monster->stats,
					       &space->player->stats);
		player_hit(space->player, dmg);
	}

	return space->occupied;
}

static bool
move_left(Monster *m, RoomMatrix *rm)
{
	m->sprite->pos.x -= TILE_DIMENSION;
	if (has_collided(m, rm)) {
	    m->sprite->pos.x += TILE_DIMENSION;
	    return false;
	}
	return true;
}

static bool
move_right(Monster *m, RoomMatrix *rm)
{
	m->sprite->pos.x += TILE_DIMENSION;
	if (has_collided(m, rm)) {
	    m->sprite->pos.x -= TILE_DIMENSION;
	    return false;
	}
	return true;
}

static bool
move_up(Monster *m, RoomMatrix *rm)
{
	m->sprite->pos.y -= TILE_DIMENSION;
	if (has_collided(m, rm)) {
	    m->sprite->pos.y += TILE_DIMENSION;
	    return false;
	}
	return true;
}

static bool
move_down(Monster *m, RoomMatrix *rm)
{
	m->sprite->pos.y += TILE_DIMENSION;
	if (has_collided(m, rm)) {
	    m->sprite->pos.y -= TILE_DIMENSION;
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
		int move = get_random(3);
		if (move == 0 && move_left(m, rm))
			break;
		else if (move == 1 && move_right(m, rm))
			break;
		else if (move == 2 && move_up(m, rm))
			break;
		else if (move == 3 && move_down(m, rm))
			break;
	}
}

static void
monster_agressive_walk(Monster *m, RoomMatrix *rm)
{
	int x_dist, y_dist;
	Position mPos;

	mPos = position_to_matrix_coords(&m->sprite->pos);

	x_dist = mPos.x - rm->playerRoomPos.x;
	y_dist = mPos.y - rm->playerRoomPos.y;


	if (abs(x_dist) > abs(y_dist)) {
		if (x_dist > 0)
			move_left(m, rm);
		else
			move_right(m, rm);
	} else {
		if (y_dist > 0)
			move_up(m, rm);
		else
			move_down(m, rm);
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
			move_right(m, rm);
		else
			move_left(m, rm);
	} else {
		if (y_dist > 0)
			move_down(m, rm);
		else
			move_up(m, rm);
	}
}

void
monster_move(Monster *m, RoomMatrix *rm)
{
	Position monsterRoomPos;

	monsterRoomPos = position_to_matrix_coords(&m->sprite->pos);
	rm->spaces[monsterRoomPos.x][monsterRoomPos.y].occupied = false;
	rm->spaces[monsterRoomPos.x][monsterRoomPos.y].monster = NULL;

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
	};

	monster_update_pos(m, m->sprite->pos);

	monsterRoomPos = position_to_matrix_coords(&m->sprite->pos);
	rm->spaces[monsterRoomPos.x][monsterRoomPos.y].occupied = true;
	rm->spaces[monsterRoomPos.x][monsterRoomPos.y].monster = m;
}

void
monster_hit(Monster *monster, unsigned int dmg)
{
	if (dmg > 0) {
		monster->hitText->active = true;
		monster->missText->active = false;
	} else {
		monster->missText->active = true;
		monster->hitText->active = false;
	}

	monster->state.current = monster->state.challenge;
}

void
monster_render(Monster *m, Camera *cam)
{
	sprite_render(m->sprite, cam);
	if (m->hitText)
		actiontext_render(m->hitText, cam);
	if (m->missText)
		actiontext_render(m->missText, cam);
}

void
monster_destroy(Monster *m)
{
	sprite_destroy(m->sprite);
	if (m->hitText)
		actiontext_destroy(m->hitText);
	if (m->missText)
		actiontext_destroy(m->missText);
	free(m);
}
