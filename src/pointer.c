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

#include <stdbool.h>
#include <stdlib.h>
#include "pointer.h"
#include "util.h"
#include "particle_engine.h"

Pointer *
pointer_create(SDL_Renderer *renderer)
{
	SDL_ShowCursor(SDL_DISABLE);

	Pointer *p = ec_malloc(sizeof(Pointer));
	p->sprite = sprite_create();
	sprite_load_texture(p->sprite, "Items/MedWep.png", 0, renderer);
	p->sprite->fixed = true;
	p->sprite->clip = (SDL_Rect) { 0, 0, 16, 16 };
	p->sprite->dim = GAME_DIMENSION;

	return p;
}

void
pointer_handle_input(Pointer *p, Input *input)
{
	// Compensate for a small offset in the sprite
	p->sprite->pos.x = input->mouseX - 6;
	p->sprite->pos.y = input->mouseY - 6;
}

void
pointer_toggle_clickable_pointer(Pointer *p, bool clickable)
{
	if (clickable)
		p->sprite->clip.x = 32;
	else
		p->sprite->clip.x = 0;
}

void
pointer_render(Pointer *p, Camera *cam)
{
	sprite_render(p->sprite, cam);
}

void
pointer_destroy(Pointer *p)
{
	SDL_ShowCursor(SDL_ENABLE);

	sprite_destroy(p->sprite);
	free(p);
}
