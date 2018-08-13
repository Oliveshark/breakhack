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

#ifndef MAP_ROOM_MODIFIERS_H_
#define	MAP_ROOM_MODIFIERS_H_

#include "vector2d.h"

// Forward declares
typedef struct Player Player;
typedef struct RoomMatrix_t RoomMatrix;

typedef enum RoomModifierType_e {
	RMOD_TYPE_NONE,
	RMOD_TYPE_WINDY,
	RMOD_TYPE_FIRE
} RoomModifierType;

typedef struct WindData {
	Vector2d direction;
} WindData;

typedef union RoomModifierDataContainer_t {
	WindData wind;
} RoomModifierDataContainer;

typedef struct RoomModifierData {
	RoomModifierType type;
	RoomModifierDataContainer data;
} RoomModifierData;

void
map_room_modifier_player_effect(Player*,
								RoomMatrix*,
								Vector2d *direction,
								void (*)(Player*, RoomMatrix*, Vector2d));

#endif // MAP_ROOM_MODIFIERS_H_
