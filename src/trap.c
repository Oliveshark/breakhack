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

#include <SDL.h>
#include "trap.h"
#include "util.h"
#include "gui.h"
#include "random.h"
#include "actiontextbuilder.h"

Trap *
trap_create()
{
	Trap *trap = ec_malloc(sizeof(Trap));
	trap->sprite = sprite_create();
	trap->sprite->animate = false;
	trap->sprite->dim = GAME_DIMENSION;
	trap->damage = 10;
	return trap;
}

void
trap_activate(Trap *trap, Player *player)
{
	if (!trap->sprite->animate) {
		gui_log("A trap is sprung!");
		trap->sprite->animate = true;
	} else {
		gui_log("You step in a trap!");
	}

	if (get_random(10) > 2 * player_has_artifact(player, TRAP_AVOIDANCE)) {
		player->stats.hp -= trap->damage;
		player_hit(player, trap->damage);
	} else {
		actiontextbuilder_create_text("Dodged", C_YELLOW, &player->sprite->pos);
		gui_log("You nimbly avoid getting hurt by the trap");
	}
}

void
trap_render(Trap *trap, Camera *cam)
{
	sprite_render(trap->sprite, cam);
}

void
trap_destroy(Trap *trap)
{
	sprite_destroy(trap->sprite);
	free(trap);
}
