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

#define ENGINEER_STATS	{ 12, 12, 5, 7, 2, 1, 1 }
#define MAGE_STATS	{ 12, 12, 5, 7, 2, 1, 1 }
#define PALADIN_STATS	{ 12, 12, 8, 9, 2, 1, 1 }
#define ROGUE_STATS	{ 12, 12, 5, 7, 3, 2, 1 }
#define WARRIOR_STATS	{ 12, 12, 8, 9, 2, 1, 1 }

static void
player_levelup(Player *player)
{
	mixer_play_effect(LEVEL_UP);

	player->stats.lvl += 1;
	player->stats.maxhp += 3;
	player->stats.dmg += 5;
	player->stats.atk += 1;
	player->stats.def += 1;

	// Limit health to 3 rows of hearts
	if (player->stats.maxhp > 72)
		player->stats.maxhp = 72;

	player->stats.hp = player->stats.maxhp;
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
	player->xp += xp_gain;
	if (player->xp >= next_level_threshold(player->stats.lvl)) {
		player_levelup(player);
		gui_log("You have reached level %u", player->stats.lvl);
	}
}

static bool 
has_collided(Player *player, RoomMatrix *matrix)
{
	bool collided = false;

	Position roomCoord = position_to_room_coords(&player->sprite->pos);
	if (roomCoord.x != matrix->roomPos.x
	    || roomCoord.y != matrix->roomPos.y) {
		return collided;
	}

	Position matrixPos = position_to_matrix_coords(&player->sprite->pos);
	RoomSpace *space = &matrix->spaces[matrixPos.x][matrixPos.y];
	collided = space->occupied;

	if (space->monster != NULL) {
		unsigned int hit = stats_fight(&player->stats,
					       &space->monster->stats);

		mixer_play_effect(SWING0 + (rand() % 3));

		monster_hit(space->monster, hit);

		if (hit > 0) {
			player->hits += 1;
			mixer_play_effect(SWORD_HIT);
		} else {
			player->misses += 1;
		}

		if (hit > 0)
			gui_log("You hit %s for %u damage",
				space->monster->lclabel, hit);
		else
			gui_log("You missed %s", space->monster->lclabel);

		if (space->monster->stats.hp <= 0) {
			unsigned int gained_xp = 5 * space->monster->stats.lvl;
			player->kills += 1;

			mixer_play_effect(DEATH);
			gui_log("You killed %s and gained %d xp",
				space->monster->lclabel, gained_xp);
			player_gain_xp(player, gained_xp);
		}
	} else if (collided) {
		mixer_play_effect(BONK);
		gui_log("Ouch! There is something in the way");
	}

	if (space->items != NULL) {
		LinkedList *items = space->items;
		while (items != NULL) {
			Item *item = items->data;
			items = items->next;
			item_collected(item, player);
		}
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

static void
move_right(Player *player, RoomMatrix *matrix)
{
	player->sprite->clip.y = 32;
	player->sprite->pos.x += TILE_DIMENSION;
	if (has_collided(player, matrix)) {
		player->sprite->pos.x -= TILE_DIMENSION;
	}
	player_step(player);
}

static void
move_up(Player *player, RoomMatrix *matrix)
{
	player->sprite->clip.y = 48;
	player->sprite->pos.y -= TILE_DIMENSION;
	if (has_collided(player, matrix)) {
		player->sprite->pos.y += TILE_DIMENSION;
	}
	player_step(player);
}

static void
move_down(Player *player, RoomMatrix *matrix)
{
	player->sprite->clip.y = 0;
	player->sprite->pos.y += TILE_DIMENSION;
	if (has_collided(player, matrix)) {
		player->sprite->pos.y -= TILE_DIMENSION;
	}
	player_step(player);
}

static void
sip_health(Player *player)
{
	if (player->potion_sips > 0) {
		--player->potion_sips;
		++player->stats.hp;
		mixer_play_effect(BUBBLE0 + (rand() % 3));
		gui_log("You take a sip of health potion");
	} else {
		gui_log("You have nothing to sip");
	}
}

static void
handle_movement_input(Player *player, RoomMatrix *matrix, SDL_Event *event)
{
	static unsigned int step = 1;
	bool moved = false;

	if (keyboard_direction_press(LEFT, event)) {
		move_left(player, matrix);
		moved = true;
	}
	if (keyboard_direction_press(RIGHT, event)) {
		move_right(player, matrix);
		moved = true;
	}
	if (keyboard_direction_press(UP, event)) {
		move_up(player, matrix);
		moved = true;
	}
	if (keyboard_direction_press(DOWN, event)) {
		move_down(player, matrix);
		moved = true;
	}
#ifdef DEBUG
	if (keyboard_mod_press(SDLK_SPACE, KMOD_CTRL, event)) {
		Position pos = player->sprite->pos;
		pos.x += 8;
		pos.y += 8;
		particle_engine_bloodspray(pos, (Dimension) { 8, 8 }, 200);
		player->stats.hp = 0;
	}
#endif // DEBUG

	if (moved) {
		player->sprite->clip.x = 16*step;
		++step;
		step = step % 4;
	}
}

static void
handle_player_input(Player *player, RoomMatrix *matrix, SDL_Event *event)
{
	if (event->type != SDL_KEYDOWN)
		return;

	if (keyboard_mod_press(SDLK_h, KMOD_SHIFT, event)) {
		sip_health(player);
	} else {
		handle_movement_input(player, matrix, event);
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
	player->gold		= 0;
	player->potion_sips	= 0;
	player->class		= class;

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
player_hit(Player *p, unsigned int dmg)
{
	if (p->stats.hp <= 0) {
		dmg = 200;
	}
	if (dmg > 0) {
		p->hitText->active = true;
		p->missText->active = false;
		Position pos = p->sprite->pos;
		pos.x += 8;
		pos.y += 8;
		particle_engine_bloodspray(pos, (Dimension) { 8, 8 }, dmg);
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
	debug("Gold: %.2f", p->gold);
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
