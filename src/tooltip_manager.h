/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2024  Linus Probert <linus.probert@gmail.com>
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

#include "camera.h"
#include "gamecontroller.h"

typedef struct Sprite Sprite;

typedef enum TooltipType {
	TOOLTIP_TYPE_HOWTO,
	TOOLTIP_TYPE_SKILL,
	TOOLTIP_TYPE_ARTIFACT,
} TooltipType;

void tooltip_manager_init(Camera *gCamera);

void tooltip_manager_set_controller_mode(GamepadType mode);

Sprite* tooltip_manager_get_tooltip(TooltipType type);

void tooltip_manager_close(void);
