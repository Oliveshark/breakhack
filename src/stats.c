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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "gui.h"
#include "stats.h"
#include "random.h"
#include "util.h"
#include "defines.h"

static int
get_attack_roll(Stats *attacker)
{
	int roll;
	int firstRoll = get_random(19) + 1;
	int secondRoll = get_random(19) + 1;

	if (attacker->advantage)
		roll = max(firstRoll, secondRoll);
	else if (attacker->disadvantage)
		roll = min(firstRoll, secondRoll);
	else
		roll = firstRoll;

	return roll + attacker->atk;
}

static int
get_defence_roll(Stats *defender)
{
	int roll;
	int firstRoll = get_random(19) + 1;
	int secondRoll = get_random(19) + 1;

	if (defender->advantage)
		roll = max(firstRoll, secondRoll);
	else if (defender->disadvantage)
		roll = min(firstRoll, secondRoll);
	else
		roll = firstRoll;

	return roll + defender->def;
}

CombatResult
stats_fight(Stats *attacker, Stats *defender)
{
	CombatResult result = { 0, false };

	int atkRoll = get_attack_roll(attacker);
	int defRoll = get_defence_roll(defender);

	if (atkRoll - attacker->atk == 20)
		result.critical = get_attack_roll(attacker) > defRoll;

	if (atkRoll >= defRoll) {
		if (attacker->dmg > 0)
			result.dmg = get_random(attacker->dmg - 1) + 1;
		else
			result.dmg = 1;

		if (result.critical) {
			result.dmg = result.dmg * 2;
			gui_log("CRITICAL HIT!");
		}

		defender->hp -= result.dmg;
	}

	return result;
}
