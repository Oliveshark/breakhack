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

#ifndef STATS_H_
#define	STATS_H_

typedef struct Stats_t {
	int maxhp;			/* Max hitpoints */
	int hp;				/* Current hit points */
	int dmg;			/* Damage modifier */
	int atk;			/* Attack rating */
	int def;			/* Defence rating */
	unsigned int speed;	/* Speed */
	unsigned int lvl;	/* Level */
	bool advantage;
	bool disadvantage;
} Stats;

typedef struct CombatResult {
	unsigned int dmg;
	bool critical;
} CombatResult;

CombatResult
stats_fight(Stats *attacker, Stats *defender);

#endif // STATS_H_
