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

#ifndef SPRITE_H_
#define	SPRITE_H_

#include <stdbool.h>

#include "texture.h"
#include "position.h"
#include "camera.h"
#include "roommatrix.h"
#include "timer.h"

typedef struct UpdateData UpdateData;

typedef enum SpriteState {
	SPRITE_STATE_FALLING,
	SPRITE_STATE_PLUMMETED,
	SPRITE_STATE_DEFAULT
} SpriteState;

typedef struct Sprite {
	SpriteState state;
	Texture* textures[2];
	SDL_Rect clip;
	bool destroyTextures;
	Position pos;
	Position offset;
	Dimension dim;
	double angle;
	SDL_Point rotationPoint;
	SDL_FlipMode flip;
	Timer *renderTimer;
	Timer *animationTimer;
	unsigned int texture_index;
	bool fixed;
	bool animate;
	bool hidden;
	void (*onRender)(Sprite*);
} Sprite;

Sprite*
sprite_create(void);

void
sprite_load_texture(Sprite *, const char *path, int index, SDL_Renderer *);

void
sprite_load_text_texture(Sprite *, const char *path, int index, int size, int outline);

void
sprite_set_texture(Sprite *, Texture *, int index);

void
sprite_update(Sprite*, UpdateData *data);

void
sprite_render(Sprite*, Camera*);

void
sprite_set_blend_mode(Sprite*, SDL_BlendMode);

void
sprite_set_alpha(Sprite*, Uint8);

void
sprite_destroy(Sprite *);

#endif // SPRITE_H_
