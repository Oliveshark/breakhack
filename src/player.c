#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "player.h"
#include "monster.h"
#include "util.h"
#include "gui.h"

#define ENGINEER_STATS	{ 12, 12, 5, 7, 2, 1, 1 }
#define MAGE_STATS	{ 12, 12, 5, 7, 2, 1, 1 }
#define PALADIN_STATS	{ 12, 12, 5, 7, 2, 1, 1 }
#define ROGUE_STATS	{ 12, 12, 5, 7, 2, 2, 1 }
#define WARRIOR_STATS	{ 12, 12, 5, 7, 2, 1, 1 }

static bool 
has_collided(Player *player, RoomMatrix *matrix)
{
	bool collided = false;

	Position roomCoord = position_to_room_coords(&player->sprite->pos);
	if (roomCoord.x != matrix->roomPos.x || roomCoord.y != matrix->roomPos.y) {
		return collided;
	}

	Position matrixPos = position_to_matrix_coords(&player->sprite->pos);
	RoomSpace *space = &matrix->spaces[matrixPos.x][matrixPos.y];
	collided = space->occupied;

	if (space->monster != NULL) {
		unsigned int hit = stats_fight(&player->stats,
					       &space->monster->stats);
		monster_hit(space->monster, hit);

		if (hit > 0)
			player->hits += 1;
		else
			player->misses += 1;

		if (hit > 0)
			gui_log("You hit '%s' for %u damage", space->monster->label, hit);
		else
			gui_log("You missed '%s'", space->monster->label);

		if (space->monster->stats.hp <= 0) {
			// TODO(Linus): This needs some love later on.
			player->kills += 1;
			player->xp += 10;

			gui_log("You killed '%s' and gained %d xp", space->monster->label, 10);
		}
	} else if (collided) {
		gui_log("Ouch! There is something in the way");
	}

	return collided;
}

static void
player_step(Player *p)
{
	p->total_steps++;
	p->steps++;
	p->missText->pos = p->sprite->pos;
	p->hitText->pos = p->sprite->pos;
}

static void
move_left(Player *player, RoomMatrix *matrix)
{
	player->sprite->clip.y = 16;
	player->sprite->pos.x -= TILE_DIMENSION;
	if (has_collided(player, matrix)) {
		player->sprite->pos.x += TILE_DIMENSION;
	}
	player_step(player);
}

static
void move_right(Player *player, RoomMatrix *matrix)
{
	player->sprite->clip.y = 32;
	player->sprite->pos.x += TILE_DIMENSION;
	if (has_collided(player, matrix)) {
		player->sprite->pos.x -= TILE_DIMENSION;
	}
	player_step(player);
}

static
void move_up(Player *player, RoomMatrix *matrix)
{
	player->sprite->clip.y = 48;
	player->sprite->pos.y -= TILE_DIMENSION;
	if (has_collided(player, matrix)) {
		player->sprite->pos.y += TILE_DIMENSION;
	}
	player_step(player);
}

static
void move_down(Player *player, RoomMatrix *matrix)
{
	player->sprite->clip.y = 0;
	player->sprite->pos.y += TILE_DIMENSION;
	if (has_collided(player, matrix)) {
		player->sprite->pos.y -= TILE_DIMENSION;
	}
	player_step(player);
}

static
void handle_player_input(Player *player, RoomMatrix *matrix, SDL_Event *event)
{
	static unsigned int step = 1;

	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym) {
			case SDLK_LEFT:
			case SDLK_h:
				move_left(player, matrix);
				break;
			case SDLK_RIGHT:
			case SDLK_l:
				move_right(player, matrix);
				break;
			case SDLK_UP:
			case SDLK_k:
				move_up(player, matrix);
				break;
			case SDLK_DOWN:
			case SDLK_j:
				move_down(player, matrix);
				break;
		}
		player->sprite->clip.x = 16*step;
		if (step == 3)
			step = 0;
		else
			++step;
	}
}

static void
player_load_texts(Player *p, SDL_Renderer *renderer)
{
	ActionText *t = actiontext_create();
	actiontext_load_font(t, "assets/GUI/SDS_6x6.ttf", 14);
	t->color = (SDL_Color) { 255, 100, 0, 255 };
	actiontext_set_text(t, "HIT", renderer);
	t->pos = p->sprite->pos;
	p->hitText = t;

	t = actiontext_create();
	actiontext_load_font(t, "assets/GUI/SDS_6x6.ttf", 14);
	t->color = (SDL_Color) { 255, 255, 0, 255 };
	actiontext_set_text(t, "MISS", renderer);
	t->pos = p->sprite->pos;
	p->missText = t;
}

Player* 
player_create(class_t class, SDL_Renderer *renderer)
{
	Player *player = malloc(sizeof(Player));
	player->sprite = sprite_create();
	player->total_steps	= 0;
	player->steps		= 0;
	player->xp		= 0;
	player->hits		= 0;
	player->kills		= 0;
	player->misses		= 0;

	char asset[100];
	switch (class) {
		case ENGINEER:
			m_strcpy(asset, 100, "assets/Commissions/Engineer.png");
			player->stats = (Stats) ENGINEER_STATS;
			break;
		case MAGE:
			m_strcpy(asset, 100, "assets/Commissions/Mage.png");
			player->stats = (Stats) MAGE_STATS;
			break;
		case PALADIN:
			m_strcpy(asset, 100, "assets/Commissions/Paladin.png");
			player->stats = (Stats) PALADIN_STATS;
			break;
		case ROGUE:
			m_strcpy(asset, 100, "assets/Commissions/Rogue.png");
			player->stats = (Stats) ROGUE_STATS;
			break;
		case WARRIOR:
			m_strcpy(asset, 100, "assets/Commissions/Warrior.png");
			player->stats = (Stats) WARRIOR_STATS;
			break;
	}

	sprite_load_texture(player->sprite, asset, 0, renderer);
	player->sprite->pos = (Position) { TILE_DIMENSION, TILE_DIMENSION };
	player->sprite->clip = (SDL_Rect) { 0, 0, 16, 16 };
	player->sprite->textures[0]->dim = (Dimension) {
		TILE_DIMENSION, TILE_DIMENSION };
	player->handle_event = &handle_player_input;

	player_load_texts(player, renderer);

	return player;
}

void
player_hit(Player *p, unsigned int dmg)
{
	if (dmg > 0) {
		p->hitText->active = true;
		p->missText->active = false;
	} else {
		p->missText->active = true;
		p->hitText->active = false;
	}
}

void
player_render(Player *player, Camera *cam)
{
	sprite_render(player->sprite, cam);
	actiontext_render(player->hitText, cam);
	actiontext_render(player->missText, cam);
}

static void
player_print(Player *p)
{
	Position roomPos = position_to_matrix_coords(&p->sprite->pos);
	Position pos = p->sprite->pos;

	debug("\n");
	debug("--------=== <[ Player Stats ]> ===--------");
	debug("HP:    %d", p->stats.hp);
	debug("Level: %u\tXP:\t%u", p->stats.lvl, p->xp);
	debug("Hits:  %u\tMisses:\t%u", p->hits, p->misses);
	debug("Kills: %u", p->kills);
	debug("Steps: %u", p->total_steps);
	debug("Pos:   %dx%d\tRoomPos: %dx%d", pos.x, pos.y,
	       roomPos.x, roomPos.y);
	debug("------------------------------------------");
}

void
player_reset_steps(Player *p)
{
	p->steps = 0;
	player_print(p);
}

void
player_destroy(Player *player)
{
	if (player->sprite)
		sprite_destroy(player->sprite);
	actiontext_destroy(player->hitText);
	actiontext_destroy(player->missText);

	free(player);
}
