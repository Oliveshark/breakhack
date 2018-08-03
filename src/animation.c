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

#include "animation.h"
#include "timer.h"
#include "camera.h"
#include "sprite.h"
#include "util.h"

Animation *
animation_create(unsigned int clipCount)
{
	Animation *animation = ec_malloc(sizeof(Animation)
					 + clipCount * sizeof(AnimationClip));
	animation->clipTimer = timer_create();
	animation->clipCount = clipCount;
	animation->currentClip = 0;
	animation->loop = true;
	animation->running = false;
	animation->sprite = sprite_create();
	return animation;
}

void
animation_load_texture(Animation *animation, const char *path, SDL_Renderer *renderer)
{
	sprite_load_texture(animation->sprite, path, 0, renderer);
}

void
animation_update(Animation *animation)
{
	if (!animation->running) {
		return;
	}

	if (!timer_started(animation->clipTimer)) {
		timer_start(animation->clipTimer);
	}

	if (timer_get_ticks(animation->clipTimer)
	    > animation->clips[animation->currentClip].renderTime)
	{
		animation->currentClip++;
		if (animation->currentClip >= animation->clipCount) {
			animation->currentClip = 0;
			if (!animation->loop) {
				animation_stop(animation);
				return;
			}
			timer_start(animation->clipTimer);
		}
	}

	animation->sprite->clip = (SDL_Rect) {
		animation->clips[animation->currentClip].x,
		animation->clips[animation->currentClip].y,
		animation->clips[animation->currentClip].w,
		animation->clips[animation->currentClip].h
	};
}

void
animation_render(Animation *animation, Camera *camera)
{
	if (!animation->running) {
		return;
	}

	sprite_render(animation->sprite, camera);
}

void
animation_set_frames(Animation *animation, AnimationClip clips[])
{
	for (size_t i = 0; i < animation->clipCount; i++) {
		animation->clips[i] = clips[i];
	}
}

void
animation_run(Animation *a)
{
	a->running = true;
}

void
animation_stop(Animation *a)
{
	a->running = false;
	a->currentClip = 0;
	timer_stop(a->clipTimer);
}

void
animation_destroy(Animation *animation)
{
	timer_destroy(animation->clipTimer);
	sprite_destroy(animation->sprite);
	free(animation);
}

