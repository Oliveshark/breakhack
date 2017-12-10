#include <string.h>
#include "player.h"

static
bool has_collided(Sprite *sprite, RoomMatrix *matrix)
{
	Position roomCoord = position_to_room_coords(&sprite->pos);
	if (roomCoord.x != matrix->roomPos.x || roomCoord.y != matrix->roomPos.y) {
		return false;
	}

	Position matrixPos = position_to_matrix_coords(&sprite->pos);
	return matrix->spaces[matrixPos.x][matrixPos.y].occupied;
}

static
void move_left(Sprite *sprite, RoomMatrix *matrix)
{
	sprite->texture->clip.y = 16;
	sprite->pos.x -= TILE_DIMENSION;
	if (has_collided(sprite, matrix))
		sprite->pos.x += TILE_DIMENSION;
}

static
void move_right(Sprite *sprite, RoomMatrix *matrix)
{
	sprite->texture->clip.y = 32;
	sprite->pos.x += TILE_DIMENSION;
	if (has_collided(sprite, matrix))
		sprite->pos.x -= TILE_DIMENSION;
}

static
void move_up(Sprite *sprite, RoomMatrix *matrix)
{
	sprite->texture->clip.y = 48;
	sprite->pos.y -= TILE_DIMENSION;
	if (has_collided(sprite, matrix))
		sprite->pos.y += TILE_DIMENSION;
}

static
void move_down(Sprite *sprite, RoomMatrix *matrix)
{
	sprite->texture->clip.y = 0;
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
				move_left(sprite, matrix);
				break;
			case SDLK_RIGHT:
				move_right(sprite, matrix);
				break;
			case SDLK_UP:
				move_up(sprite, matrix);
				break;
			case SDLK_DOWN:
				move_down(sprite, matrix);
				break;
		}
		sprite->texture->clip.x = 16*step;
		if (step == 3)
			step = 0;
		else
			++step;
	}
}

Sprite* player_create(class_t class, SDL_Renderer *renderer)
{
	Sprite *player = sprite_create();
	char asset[100];

	switch (class) {
		case ENGINEER:
			strcpy(asset, "assets/Commissions/Engineer.png");
			break;
		case MAGE:
			strcpy(asset, "assets/Commissions/Mage.png");
			break;
		case PALADIN:
			strcpy(asset, "assets/Commissions/Paladin.png");
			break;
		case ROGUE:
			strcpy(asset, "assets/Commissions/Rogue.png");
			break;
		case WARRIOR:
			strcpy(asset, "assets/Commissions/Warrior.png");
			break;
	}

	sprite_load_texture(player, asset, renderer);
	player->pos = (Position) { TILE_DIMENSION, TILE_DIMENSION };
	player->texture->clip = (SDL_Rect) { 0, 0, 16, 16 };
	player->texture->dim = (Dimension) { TILE_DIMENSION, TILE_DIMENSION };
	player->handle_event = &handle_player_input;

	return player;
}
