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
#include "map_room_modifiers.h"
#include "player.h"
#include "roommatrix.h"
#include "position.h"

void
map_room_modifier_player_effect(Player *player,
				RoomMatrix *matrix,
				Vector2d *direction,
				void (*move_cb)(Player*, RoomMatrix*, Vector2d))
{
	Position matrixPos = position_to_matrix_coords(&player->sprite->pos);
	if (matrix->modifier->type == RMOD_TYPE_WINDY
	    && !vector2d_equals(*direction, VECTOR2D_NODIR)
	    && matrixPos.x > 0 && matrixPos.x < MAP_ROOM_WIDTH-1
	    && matrixPos.y > 0 && matrixPos.y < MAP_ROOM_HEIGHT-1)
	{
		if (!vector2d_is_opposite(*direction, matrix->modifier->data.wind.direction)) {
			move_cb(player, matrix, matrix->modifier->data.wind.direction);
		}
	}
}
