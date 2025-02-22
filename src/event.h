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

#pragma once

#include "player.h"

typedef enum EventType {
	MONSTER_KILLED_EVENT
} EventType;

typedef struct MonsterKilledEvent {
	EventType *type;
	Player *player;
	Monster *monster;
} MonsterKilledEvent;

typedef union Event {
	EventType type;
	MonsterKilledEvent monsterKilled;
} Event;

typedef void (*EventCallback)(Event*);

void
event_register_listener(EventCallback cb);

void
event_clear_listeners(void);

void
event_trigger(Event *event);
