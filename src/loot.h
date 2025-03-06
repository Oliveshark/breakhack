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
#ifndef _LOOT_H_
#define _LOOT_H_

#include "player.h"
#include "monster.h"
#include "roommatrix.h"

/**
 * \brief Drop loot from a monster
 *
 * This function will drop loot from a monster. The loot is dropped in the
 * roommatrix and the map.
 *
 * \param m The monster
 * \param rm The roommatrix
 * \param map The map
 * \param player The player
 */
void loot_drop(Monster *monster, Map *map, RoomMatrix *rm, Player *player);

#endif  // _LOOT_H_

