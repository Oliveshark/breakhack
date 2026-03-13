/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2025  Linus Probert <linus.probert@gmail.com>
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
#include "debug.h"
#include "../sprite.h"
#include "../texturecache.h"

static Sprite *sprite = NULL;

void
debug_update(UpdateData *ud)
{
	/* Kill a monster when clicked */
	if (input_mousebutton_is_pressed(ud->input, MBUTTON_LEFT)) {
		Position mpos = POS(ud->input->mouseX, ud->input->mouseY);

		Position mat_pos = position_to_matrix_coords(&mpos);
		Monster *monster = ud->matrix->spaces[mat_pos.x][mat_pos.y].monster;
		if (monster) {
			monster->stats.hp = 0;
		} else {
			if (sprite) {
				sprite_destroy(sprite);
			}
			sprite = sprite_create();
			sprite->textures[0] = texturecache_add("Items/Amulet.png");
			sprite->clip = CLIP16(0, 0);
			sprite->dim = DIM(4, 4);
			sprite->pos = mpos;

			Destination dest = {
			    .pos = POS(mpos.x + 64, mpos.y + 64),
			    .dim = DIM(32, 32),
			    .angle = 0.0f,
			    .time_ms = 400,
			};

			sprite_interpolate_to(sprite, &dest);
		}
	}

	if (sprite) {
		sprite_update(sprite, ud);
	}
}

void
debug_render(Camera *cam)
{
	if (sprite)
		sprite_render(sprite, cam);
}
