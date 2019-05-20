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

#include "gui.h"
#include "monster.h"
#include "roommatrix.h"
#include "effect_util.h"

void
effect_damage_surroundings(Position *pos,
			   RoomMatrix *rm,
			   Stats *attackerStats,
			   unsigned int radius,
			   unsigned int pushRadius,
			   bool damagePlayer)
{
	Position roomPos = position_to_matrix_coords(pos);
	for (Sint32 i = -radius; i <= (Sint32) radius; ++i) {
		for (Sint32 j = -radius; j <= (Sint32) radius; ++j) {
			if (i == 0 && j == 0)
				continue;

			Position matrixPos = POS(roomPos.x + i, roomPos.y + j);
			if (!position_in_roommatrix(&matrixPos))
				continue;

			RoomSpace *r = &rm->spaces[matrixPos.x][matrixPos.y];
			if (r->monster) {
				CombatResult result = stats_fight(attackerStats, &r->monster->stats);
				monster_hit(r->monster, result.dmg, result.critical);
				gui_log("%s takes %d damage from the explosion", r->monster->label, result.dmg);

				Vector2d dir = vector2d_to_direction(&VEC2D((float) i, (float) j));
				for (unsigned int k = 0; k < pushRadius; ++k) {
					monster_push(r->monster,
						     roommatrix_get_player(rm),
						     rm,
						     dir);
				}
			} else if (r->player && damagePlayer) {
				CombatResult result = stats_fight(attackerStats, &r->player->stats);
				player_hit(r->player, result.dmg);
				gui_log("You take %d damage from the explosion", result.dmg);
			}
		}
	}
}
