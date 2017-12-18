#include <string.h>
#include "player.h"
#include "monster.h"

static Stats classStats[] = {
	(Stats) { 11, 5, 7, 2, 1, 1 }, /* ENGINEER */
	(Stats) { 11, 5, 7, 2, 1, 1 }, /* MAGE */
	(Stats) { 11, 5, 7, 2, 1, 1 }, /* PALADIN */
	(Stats) { 11, 5, 7, 2, 2, 1 }, /* ROGUE */
	(Stats) { 11, 5, 7, 2, 1, 1 }, /* WARRIOR */
};

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
		if (hit > 0) {
			space->monster->hitText->active = true;
			space->monster->missText->active = false;
		} else {
			space->monster->missText->active = true;
			space->monster->hitText->active = false;
		}

		if (space->monster->stats.hp <= 0) {
			// TODO(Linus): This needs some love later on.
			player->xp += 10;
		}
	}

	return collided;
}

static void
player_step(Player *p, RoomMatrix* m)
{
	p->total_steps++;
	p->steps++;
}

static void
move_left(Player *player, RoomMatrix *matrix)
{
	player->sprite->clip.y = 16;
	player->sprite->pos.x -= TILE_DIMENSION;
	if (has_collided(player, matrix)) {
		player->sprite->pos.x += TILE_DIMENSION;
	}
	player_step(player, matrix);
}

static
void move_right(Player *player, RoomMatrix *matrix)
{
	player->sprite->clip.y = 32;
	player->sprite->pos.x += TILE_DIMENSION;
	if (has_collided(player, matrix)) {
		player->sprite->pos.x -= TILE_DIMENSION;
	}
	player_step(player, matrix);
}

static
void move_up(Player *player, RoomMatrix *matrix)
{
	player->sprite->clip.y = 48;
	player->sprite->pos.y -= TILE_DIMENSION;
	if (has_collided(player, matrix)) {
		player->sprite->pos.y += TILE_DIMENSION;
	}
	player_step(player, matrix);
}

static
void move_down(Player *player, RoomMatrix *matrix)
{
	player->sprite->clip.y = 0;
	player->sprite->pos.y += TILE_DIMENSION;
	if (has_collided(player, matrix)) {
		player->sprite->pos.y -= TILE_DIMENSION;
	}
	player_step(player, matrix);
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

Player* 
player_create(class_t class, SDL_Renderer *renderer)
{
	Player *player = malloc(sizeof(Player));
	player->sprite = sprite_create();
	player->total_steps = 0;
	player->steps = 0;
	player->xp = 0;

	char asset[100];
	switch (class) {
		case ENGINEER:
			strcpy(asset, "assets/Commissions/Engineer.png");
			player->stats = classStats[ENGINEER];
			break;
		case MAGE:
			strcpy(asset, "assets/Commissions/Mage.png");
			player->stats = classStats[MAGE];
			break;
		case PALADIN:
			strcpy(asset, "assets/Commissions/Paladin.png");
			player->stats = classStats[PALADIN];
			break;
		case ROGUE:
			strcpy(asset, "assets/Commissions/Rogue.png");
			player->stats = classStats[ROGUE];
			break;
		case WARRIOR:
			strcpy(asset, "assets/Commissions/Warrior.png");
			player->stats = classStats[WARRIOR];
			break;
	}

	sprite_load_texture(player->sprite, asset, 0, renderer);
	player->sprite->pos = (Position) { TILE_DIMENSION, TILE_DIMENSION };
	player->sprite->clip = (SDL_Rect) { 0, 0, 16, 16 };
	player->sprite->textures[0]->dim = (Dimension) {
		TILE_DIMENSION, TILE_DIMENSION };
	player->handle_event = &handle_player_input;

	return player;
}

void
player_print(Player *p)
{
	Position roomPos = position_to_matrix_coords(&p->sprite->pos);
	Position pos = p->sprite->pos;

	printf("\n");
	printf("--------=== <[ Player Stats ]> ===--------\n");
	printf("HP: %d\n", p->stats.hp);
	printf("Level: %d   XP: %u\n", p->stats.lvl, p->xp);
	printf("Pos: %dx%d  RoomPos: %dx%d\n", pos.x, pos.y, roomPos.x, roomPos.y);
	printf("Steps: %u\n", p->total_steps);
	printf("------------------------------------------\n");
}

void
player_destroy(Player *player)
{
	if (player->sprite)
		sprite_destroy(player->sprite);

	free(player);
}
