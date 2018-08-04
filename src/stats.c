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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL_ttf.h>

#include "gui.h"
#include "stats.h"
#include "random.h"
#include "util.h"
#include "defines.h"

unsigned int
stats_fight(Stats *attacker, Stats *defender)
{
	int atkRoll, defRoll, dmgRoll;
	bool critical = false;

	if (attacker->advantage)
		atkRoll = max(get_random(19), get_random(19)) + 1;
	else if (attacker->disadvantage)
		atkRoll = min(get_random(19), get_random(19)) + 1;
	else
		atkRoll = get_random(19) + 1;

	if (atkRoll == 20)
		critical = true;
	atkRoll += attacker->atk;

	if (defender->advantage)
		defRoll = max(get_random(19), get_random(19)) + 1 + defender->def;
	else if (defender->disadvantage)
		defRoll = min(get_random(19), get_random(19)) + 1 + defender->def;
	else
		defRoll = get_random(19) + 1 + defender->def;
	dmgRoll = 0;

	if (atkRoll >= defRoll) {
		if (attacker->dmg > 0)
			dmgRoll = get_random(attacker->dmg) + 1;
		else
			dmgRoll = get_random(attacker->dmg - 1) + 1;

		if (critical) {
			dmgRoll = dmgRoll * 2;
			gui_log("CRITICAL HIT!");
		}

		defender->hp -= dmgRoll;
	}

	return dmgRoll;
}
