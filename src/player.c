#include <string.h>
#include "player.h"

static
void handle_player_input(Sprite *sprite, SDL_Event *event)
{
	unsigned int width = sprite->texture->dim.width;
	unsigned int height = sprite->texture->dim.height;
	static unsigned int step = 1;

	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym) {
		case SDLK_LEFT:
			sprite->texture->clip.y = 16;
			sprite->pos.x -= width;
			break;
		case SDLK_RIGHT:
			sprite->texture->clip.y = 32;
			sprite->pos.x += width;
			break;
		case SDLK_UP:
			sprite->texture->clip.y = 48;
			sprite->pos.y -= height;
			break;
		case SDLK_DOWN:
			sprite->texture->clip.y = 0;
			sprite->pos.y += height;
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
	player->pos = (Position) { 64, 64 };
	player->texture->clip = (SDL_Rect) { 0, 0, 16, 16 };
	player->texture->dim = (Dimension) { 64, 64 };
	player->handle_event = &handle_player_input;

	return player;
}
