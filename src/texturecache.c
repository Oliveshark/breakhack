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

#include "texturecache.h"
#include "hashtable.h"
#include "util.h"

static Hashtable *textures = NULL;
static SDL_Renderer *renderer;

void
texturecache_init(SDL_Renderer *rend)
{
	textures = ht_create(50);
	renderer = rend;
}

Texture*
texturecache_add(const char *path)
{
	Texture *t = ht_get(textures, path);
	if (!t) {
		t = texture_create();
		texture_load_from_file(t, path, renderer);
		ht_set(textures, path, t);
		debug("Cached texture: %s", path);
	} else {
		debug("Retrieved cached texture: %s", path);
	}

	return t;
}

Texture*
texturecache_get(const char *path)
{
	Texture *t = ht_get(textures, path);
	if (!t)
		fatal("Texture not loaded: %s", path);
	return t;
}

void
texturecache_close(void)
{
	ht_destroy_custom(textures, (void(*)(void*)) texture_destroy);
}
