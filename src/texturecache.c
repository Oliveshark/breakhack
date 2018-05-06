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

#include <SDL.h>
#include <assert.h>
#include "texturecache.h"
#include "hashtable.h"
#include "defines.h"
#include "util.h"
#include "timer.h"

static Hashtable *textures = NULL;
static SDL_Renderer *renderer = NULL;

typedef struct TexureContainer {
	Texture *texture;
} TextureContainer;

static void
texturecache_texture_container_destroy(TextureContainer *);

void
texturecache_init(SDL_Renderer *rend)
{
	textures = ht_create(50);
	renderer = rend;
}

Texture*
texturecache_add(const char *path)
{
	TextureContainer *tc = ht_get(textures, path);
	if (!tc) {
		tc = ec_malloc(sizeof(TextureContainer));
		tc->texture = texture_create();
		texture_load_from_file(tc->texture, path, renderer);
		ht_set(textures, path, tc);
		debug("Cached texture: %s", path);
	} else {
		debug("Retrieved cached texture: %s", path);
	}

	return tc->texture;
}

Texture*
texturecache_get(const char *path)
{
	TextureContainer *tc = ht_get(textures, path);
	if (!tc)
		fatal("Texture not loaded: %s", path);

	return tc->texture;
}

bool
texturecache_remove(Texture **texture)
{
	TextureContainer *tc = ht_get(textures, (*texture)->path);
	if (!tc)
		return false;
	assert(tc->texture == *texture);
	texturecache_texture_container_destroy(tc);
	*texture = NULL;
	return true;
}

static void
texturecache_texture_container_destroy(TextureContainer *tc)
{
	texture_destroy(tc->texture);
	free(tc);
}

void
texturecache_close(void)
{
	ht_destroy_custom(textures, (void(*)(void*)) texturecache_texture_container_destroy);
}
