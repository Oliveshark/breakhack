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

#ifndef GUI_H_
#define	GUI_H_

#define LOG_LINES_COUNT	10
#define LOG_FONT_SIZE	8
#define LABEL_FONT_SIZE	8

#include "linkedlist.h"
#include "sprite.h"
#include "camera.h"
#include "player.h"

typedef enum Label_e {
	LEVEL_LABEL,
	CURRENT_XP_LABEL,
	GOLD_LABEL,
	DUNGEON_LEVEL_LABEL,
	HEALTH_POTION_LABEL,
	LABEL_COUNT
} LabelIndex;

typedef struct {
	LinkedList *sprites;
	LinkedList *health;
	LinkedList *xp_bar;
	Sprite *labels[LABEL_COUNT];
	Texture *log_lines[LOG_LINES_COUNT];
} Gui;

Gui*
gui_create(SDL_Renderer *renderer);

void
gui_update_player_stats(Gui*, Player*, Map*, SDL_Renderer*);

void
gui_render_panel(Gui*, unsigned int width, unsigned int height, Camera*);

void
gui_render_log(Gui*, unsigned int width, unsigned int height, Camera*);

void
gui_log(const char *fmt, ...);

void
gui_destroy(Gui*);

#endif // GUI_H_
