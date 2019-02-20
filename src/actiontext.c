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

#include <stdlib.h>
#include "actiontext.h"
#include "util.h"
#include "update_data.h"

ActionText*
actiontext_create(Sprite *sprite)
{
	ActionText *t = ec_malloc(sizeof(ActionText));
	t->pos = (Position) { 0, 0 };
	t->sprite = sprite;
	t->timer = _timer_create();
	t->dead = false;
	t->velocity = (Vector2d) { 0, -100 };
	t->color = C_WHITE;
	return t;
}

void
actiontext_update(ActionText *t, UpdateData *data)
{
	t->sprite->pos.x += (int) (t->velocity.x * data->deltatime);
	t->sprite->pos.y += (int) (t->velocity.y * data->deltatime);
}

void
actiontext_render(ActionText *t, Camera *cam)
{
	if (t->dead)
		return;

	if (!t->dead && !timer_started(t->timer))
		timer_start(t->timer);

	if (timer_get_ticks(t->timer) < 500) {
		sprite_render(t->sprite, cam);
	} else {
		timer_stop(t->timer);
		t->dead = true;
	}
}

void
actiontext_destroy(ActionText *t)
{
	sprite_destroy(t->sprite);
	timer_destroy(t->timer);
	free(t);
}
