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

#ifndef UPDATE_DATA_H_
#define	UPDATE_DATA_H_

#include "player.h"
#include "map.h"
#include "roommatrix.h"
#include "gui.h"
#include "camera.h"

typedef struct UpdateData {
	Player *player;
	Map *map;
	RoomMatrix *matrix;
	Gui *gui;
	Camera *cam;
	Input *input;
	float deltatime;
} UpdateData;

#endif // UPDATE_DATA_H_
