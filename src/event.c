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

#include "event.h"
#include "util.h"
#include "linkedlist.h"

static LinkedList *callbacks = NULL;

void
event_register_listener(EventCallback cb)
{
    linkedlist_append(&callbacks, cb);
}

void
event_clear_listeners(void)
{
    while (callbacks)
        linkedlist_pop(&callbacks);
}

void
event_trigger(Event *event)
{
    LinkedList *cbs = callbacks;
    while (cbs) {
        ((EventCallback) cbs->data)(event);
        cbs = cbs->next;
    }
}