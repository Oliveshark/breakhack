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

#define EVENT_MESSAGE_DISPLAY_TIME	2500
#define EVENT_MESSAGE_FONT_SIZE		20

#include "linkedlist.h"
#include "sprite.h"
#include "camera.h"
#include "player.h"
#include "timer.h"

typedef enum Label_e {
	LEVEL_LABEL,
	CURRENT_XP_LABEL,
	GOLD_LABEL,
	DUNGEON_LEVEL_LABEL,
	HEALTH_POTION_LABEL,
	DAGGER_LABEL,
	LABEL_COUNT
} LabelIndex;

typedef struct {
	LinkedList *sprites;
	LinkedList *health;
	LinkedList *xp_bar;
	Sprite *bottomFrame;
	Sprite *rightFrame;
	Sprite *labels[LABEL_COUNT];
	Texture *log_lines[LOG_LINES_COUNT];
	Texture *event_message;
	Timer *event_message_timer;
} Gui;

Gui*
gui_create(Camera *);

void
gui_update_player_stats(Gui*, Player*, Map*, SDL_Renderer*);

void
gui_render_panel(Gui*, Camera*);

void
gui_render_log(Gui*, Camera*);

void
gui_render_event_message(Gui*, Camera*);

void
gui_log(const char *fmt, ...);

void
gui_event_message(const char *fmt, ...);

void
gui_clear_message_log(void);

void
gui_destroy(Gui*);

#endif // GUI_H_
