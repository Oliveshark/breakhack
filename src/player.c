#include <string.h>
#include "player.h"

static Stats classStats[] = {
	(Stats) { 11, 11, 11, 1 }, /* ENGINEER */
	(Stats) { 11, 11, 11, 1 }, /* MAGE */
	(Stats) { 11, 11, 11, 1 }, /* PALADIN */
	(Stats) { 11, 11, 11, 2 }, /* ROGUE */
	(Stats) { 11, 11, 11, 1 }, /* WARRIOR */
};

static bool 
has_collided(Sprite *sprite, RoomMatrix *matrix)
{
	Position roomCoord = position_to_room_coords(&sprite->pos);
	if (roomCoord.x != matrix->roomPos.x || roomCoord.y != matrix->roomPos.y) {
		return false;
	}

	Position matrixPos = position_to_matrix_coords(&sprite->pos);
	return matrix->spaces[matrixPos.x][matrixPos.y].occupied;
}

static void
move_left(Sprite *sprite, RoomMatrix *matrix)
{
	sprite->clip.y = 16;
	sprite->pos.x -= TILE_DIMENSION;
	if (has_collided(sprite, matrix))
		sprite->pos.x += TILE_DIMENSION;
}

static
void move_right(Sprite *sprite, RoomMatrix *matrix)
{
	sprite->clip.y = 32;
	sprite->pos.x += TILE_DIMENSION;
	if (has_collided(sprite, matrix))
		sprite->pos.x -= TILE_DIMENSION;
}

static
void move_up(Sprite *sprite, RoomMatrix *matrix)
{
	sprite->clip.y = 48;
	sprite->pos.y -= TILE_DIMENSION;
	if (has_collided(sprite, matrix))
		sprite->pos.y += TILE_DIMENSION;
}

static
void move_down(Sprite *sprite, RoomMatrix *matrix)
{
	sprite->clip.y = 0;
	sprite->pos.y += TILE_DIMENSION;
	if (has_collided(sprite, matrix))
		sprite->pos.y -= TILE_DIMENSION;
}

static
void handle_player_input(Sprite *sprite, RoomMatrix *matrix, SDL_Event *event)
{
	static unsigned int step = 1;

	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym) {
			case SDLK_LEFT:
			case SDLK_h:
				move_left(sprite, matrix);
				break;
			case SDLK_RIGHT:
			case SDLK_l:
				move_right(sprite, matrix);
				break;
			case SDLK_UP:
			case SDLK_k:
				move_up(sprite, matrix);
				break;
			case SDLK_DOWN:
			case SDLK_j:
				move_down(sprite, matrix);
				break;
		}
		sprite->clip.x = 16*step;
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
	player->sprite->handle_event = &handle_player_input;

	return player;
}

void
player_destroy(Player *player)
{
	if (player->sprite)
		sprite_destroy(player->sprite);

	free(player);
}
