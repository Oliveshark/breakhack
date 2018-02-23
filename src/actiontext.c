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

ActionText*
actiontext_create()
{
	ActionText *t = ec_malloc(sizeof(ActionText));
	t->pos = (Position) { 0, 0 };
	t->texture = texture_create();
	t->timer = timer_create();
	t->active = false;
	t->color = (SDL_Color) { 255, 255, 255, 255 };
	return t;
}

void
actiontext_load_font(ActionText *t, const char *path, unsigned int size)
{
	texture_load_font(t->texture, path, size);
}

void
actiontext_set_text(ActionText *t, const char *text, SDL_Renderer *renderer)
{
	texture_load_from_text(t->texture, text, t->color, renderer);
}

void
actiontext_render(ActionText *t, Camera *cam)
{
	if (!t->active)
		return;

	if (t->active && !timer_started(t->timer))
		timer_start(t->timer);

	Position cameraPos = camera_to_camera_position(cam, &t->pos);
	SDL_Rect box = { cameraPos.x, cameraPos.y, t->texture->dim.width, t->texture->dim.height };
	if (timer_get_ticks(t->timer) < 300) {
		texture_render(t->texture, &box, cam);
	} else {
		timer_stop(t->timer);
		t->active = false;
	}
}

void
actiontext_destroy(ActionText *t)
{
	texture_destroy(t->texture);
	timer_destroy(t->timer);
	free(t);
}
