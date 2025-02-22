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

#ifndef _ANIMATION_H
#define	_ANIMATION_H

#include <SDL3/SDL.h>
#include <stdbool.h>

typedef struct Timer Timer;
typedef struct Camera Camera;
typedef struct Sprite Sprite;

typedef struct AnimationClip
{
	unsigned int x;
	unsigned int y;
	unsigned int w;
	unsigned int h;
	unsigned int renderTime;
} AnimationClip;

typedef struct Animation
{
	Sprite *sprite;
	Timer *clipTimer;
	unsigned int currentClip;
	bool loop;
	unsigned int clipCount;
	bool running;
	AnimationClip clips[];
} Animation;

Animation*
animation_create(unsigned int clipCount);

void
animation_load_texture(Animation *, const char *path, SDL_Renderer*);

void
animation_set_frames(Animation*, const AnimationClip clips[]);

void
animation_run(Animation*);

void
animation_update(Animation*);

void
animation_render(Animation*, Camera*);

void
animation_stop(Animation*);

void
animation_destroy(Animation*);

#endif // _ANIMATION_H


