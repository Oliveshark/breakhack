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

#include <stdlib.h>
#include "sprite.h"
#include "util.h"
#include "update_data.h"

static Sprite *
sprite_create_default(void)
{
	Sprite *s = ec_malloc(sizeof(Sprite));
	s->state = SPRITE_STATE_DEFAULT;
	s->textures[0] = NULL;
	s->textures[1] = NULL;
	s->clip = (SDL_Rect){0, 0, 0, 0};
	s->destroyTextures = false;
	s->pos = POS(0, 0);
	s->offset = POS(0, 0);
	s->dim = DEFAULT_DIMENSION;
	s->angle = 0;
	s->rotationPoint = (SDL_Point){0, 0};
	s->flip = SDL_FLIP_NONE;
	s->renderTimer = _timer_create();
	s->animationTimer = _timer_create();
	s->texture_index = 0;
	s->fixed = false;
	s->animate = true;
	s->hidden = false;
	s->onRender = NULL;

	return s;
}

Sprite *
sprite_create(void)
{
	return sprite_create_default();
}

void
sprite_load_texture(Sprite *sprite, const char *path, int index, SDL_Renderer *renderer)
{
	if (index > 1)
		fatal("in sprite_load_texture() index out of bounds");

	if (sprite->destroyTextures && sprite->textures[index] != NULL) {
		texture_destroy(sprite->textures[index]);
		sprite->textures[index] = NULL;
	}

	sprite->textures[index] = texture_create();
	texture_load_from_file(sprite->textures[index], path, renderer);
	sprite->destroyTextures = true;
}

void
sprite_load_text_texture(Sprite *sprite, const char *path, int index, int size, int outline)
{
	if (index > 1)
		fatal("in sprite_load_texture() index out of bounds");

	if (sprite->destroyTextures && sprite->textures[index] != NULL) {
		texture_destroy(sprite->textures[index]);
		sprite->textures[index] = NULL;
	}

	sprite->textures[index] = texture_create();
	texture_load_font(sprite->textures[index], path, size, outline);
	sprite->destroyTextures = true;
}

void
sprite_set_texture(Sprite *s, Texture *t, int index)
{
	if (index > 1)
		fatal("in sprite_set_texture() index out of bounds");
	if (s->destroyTextures)
		fatal("in sprite_set_texture() sprite contains loaded textures");

	s->textures[index] = t;
}

void
sprite_set_blend_mode(Sprite *s, SDL_BlendMode mode)
{
	if (s->textures[0])
		texture_set_blend_mode(s->textures[0], mode);
	if (s->textures[1])
		texture_set_blend_mode(s->textures[1], mode);
}

void
sprite_set_alpha(Sprite *s, Uint8 alpha)
{
	if (s->textures[0])
		texture_set_alpha(s->textures[0], alpha);
	if (s->textures[1])
		texture_set_alpha(s->textures[1], alpha);
}

void
sprite_set_color_mod(Sprite *s, Uint8 r, Uint8 g, Uint8 b)
{
	if (s->textures[0])
		texture_set_color_mod(s->textures[0], r, g, b);
	if (s->textures[1])
		texture_set_color_mod(s->textures[1], r, g, b);
}

static inline void
interpolate_position(Sprite *s, float steps_remaining)
{
	Position *cpos = &s->pos;
	Position *dpos = &s->dest.pos;

	float dx = (float)(dpos->x - cpos->x) / steps_remaining;
	float dy = (float)(dpos->y - cpos->y) / steps_remaining;

	s->pos.x += (int)dx;
	s->pos.y += (int)dy;
}

static inline void
interpolate_scale(Sprite *s, float steps_remaining)
{
	Dimension *cdim = &s->dim;
	Dimension *ddim = &s->dest.dim;

	float dw = (float)(ddim->width - cdim->width) / steps_remaining;
	float dh = (float)(ddim->height - cdim->height) / steps_remaining;

	s->dim.width += (int)dw;
	s->dim.height += (int)dh;
	s->offset = POS((s->dest.dim.width - s->dim.width) / 2, (s->dest.dim.height - s->dim.height) / 2);
}

static inline void
interpolate_angle(Sprite *s, float steps_remaining)
{
	float crot = (float)s->angle;
	float drot = (float)s->dest.angle;

	float rotation = (drot - crot) / steps_remaining;
	s->angle += rotation;
}

void
sprite_update(Sprite *s, UpdateData *data)
{
	if (s->state == SPRITE_STATE_FALLING) {
		if (!timer_started(s->animationTimer)) {
			timer_start(s->animationTimer);
		} else {
			if (timer_get_ticks(s->animationTimer) > 100) {
				timer_start(s->animationTimer);
				s->angle += 60;
				s->dim.width -= 4;
				s->dim.height -= 4;
				s->pos.x += 2;
				s->pos.y += 2;
				s->rotationPoint = (SDL_Point){s->dim.width / 2, s->dim.height / 2};
			}
			if (s->dim.width < 4) {
				s->hidden = true;
				s->state = SPRITE_STATE_PLUMMETED;
			}
		}
	} else if (s->state == SPRITE_STATE_MOVING) {
		float deltatime_ms = data->deltatime * 1000;
		const float steps_remaining = s->dest.time_ms / deltatime_ms;
		if (steps_remaining <= 1) {
			/* No time left, just snap into place */
			s->pos = s->dest.pos;
			s->angle = s->dest.angle;
			s->dim = s->dest.dim;
			if (s->dest.on_complete) {
				s->dest.on_complete();
			}
			memset(&s->dest, 0, sizeof(s->dest));
			s->state = SPRITE_STATE_DEFAULT;
		} else {
			/* Interpolate position */
			interpolate_position(s, steps_remaining);
			interpolate_scale(s, steps_remaining);
			interpolate_angle(s, steps_remaining);
			s->dest.time_ms -= deltatime_ms;
		}
	}
}

void
sprite_interpolate_to(Sprite *s, Destination *dest)
{
	debug("Interpolating:");
	debug("    Pos: %dx%d -> %dx%d", s->pos.x, s->pos.y, dest->pos.x, dest->pos.y);
	s->offset = POS((dest->dim.width - s->dim.width) / 2, (dest->dim.height - s->dim.width) / 2);
	s->rotationPoint = (SDL_Point){dest->dim.width / 2, dest->dim.height / 2};
	s->state = SPRITE_STATE_MOVING;
	s->dest = *dest;
}

void
sprite_render(Sprite *s, Camera *cam)
{
	if (s->hidden)
		return;
	if (s->onRender)
		s->onRender(s);
	if (s->textures[1] && s->animate) {
		if (!timer_started(s->renderTimer))
			timer_start(s->renderTimer);

		if (timer_get_ticks(s->renderTimer) > 300) {
			timer_stop(s->renderTimer);
			timer_start(s->renderTimer);
			s->texture_index = s->texture_index == 0 ? 1 : 0;
		}
	}

	Position cameraPos;
	if (!s->fixed)
		cameraPos = camera_to_camera_position(cam, &s->pos);
	else
		cameraPos = s->pos;

	cameraPos = position_add(&cameraPos, &s->offset);

	SDL_Rect box = {cameraPos.x, cameraPos.y, s->dim.width, s->dim.height};

	if (s->angle != 0 || s->flip != SDL_FLIP_NONE) {
		texture_render_clip_ex(s->textures[s->texture_index], &box, &s->clip, s->angle, &s->rotationPoint, s->flip,
		                       cam);
	} else if (s->clip.w && s->clip.h) {
		texture_render_clip(s->textures[s->texture_index], &box, &s->clip, cam);
	} else {
		texture_render(s->textures[s->texture_index], &box, cam);
	}
}

void
sprite_destroy(Sprite *sprite)
{
	if (sprite->destroyTextures) {
		if (sprite->textures[0])
			texture_destroy(sprite->textures[0]);
		if (sprite->textures[1])
			texture_destroy(sprite->textures[1]);
	}
	timer_destroy(sprite->renderTimer);
	timer_destroy(sprite->animationTimer);
	free(sprite);
}
