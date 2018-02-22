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

#include "skillbar.h"
#include "texture.h"
#include "util.h"

static void
load_texture(SkillBar *bar, const char *path, SDL_Renderer *renderer)
{
	Texture *t = texture_create();
	texture_load_from_file(t, path, renderer);
	ht_set(bar->textures, path, t);
}

SkillBar *
skillbar_create(SDL_Renderer *renderer)
{
	SkillBar *bar = ec_malloc(sizeof(SkillBar));
	bar->textures = ht_create(10);
	load_texture(bar, "GUI/GUI0.png", renderer);
	return bar;
}

void
render(SkillBar *bar, Camera *cam)
{
	// TODO: Implement
}

void
skillbar_destroy(SkillBar *bar)
{
	ht_destroy_custom(bar->textures, (void (*)(void *)) texture_destroy);
	free(bar);
}
