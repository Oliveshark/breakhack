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
#include "gui_util.h"

static SDL_Rect frame_top_left		= { 16, 160, 16, 16 };
static SDL_Rect frame_top_right		= { 48, 160, 16, 16 };
static SDL_Rect frame_bottom_left	= { 16, 192, 16, 16 };
static SDL_Rect frame_bottom_right	= { 48, 192, 16, 16 };
static SDL_Rect frame_top		= { 32, 160, 16, 16 };
static SDL_Rect frame_bottom		= { 32, 192, 16, 16 };
static SDL_Rect frame_center		= { 32, 176, 16, 16 };
static SDL_Rect frame_left		= { 16, 176, 16, 16 };
static SDL_Rect frame_right		= { 48, 176, 16, 16 };

Sprite *
gui_util_create_frame_sprite(Uint32 width,
			     Uint32 height,
			     Camera *cam)
{
	Sprite *frame = sprite_create();
	Texture *texture = texture_create();
	texture->dim = (Dimension) {
		width * 16,
		height * 16
	};
	frame->textures[0] = texture;
	frame->destroyTextures = true;
	frame->pos = (Position) { 0, 0 };
	frame->dim = (Dimension) { width*16, height*16 };
	frame->fixed = true;
	texture_create_blank(texture,
			     SDL_TEXTUREACCESS_TARGET,
			     cam->renderer);
	Texture *source = texturecache_get("GUI/GUI0.png");

	SDL_SetRenderTarget(cam->renderer, texture->texture);
	SDL_RenderClear(cam->renderer);

	SDL_Rect box = { 0, 0, 16, 16 };
	unsigned int i, j;
	for (i = 0; i < width; ++i) {
		for (j = 0; j < height; ++j) {
			box.x = i * 16;
			box.y = j * 16;

			if (i == 0 && j == 0) {
				texture_render_clip(source,
						    &box,
						    &frame_top_left,
						    cam);
			} else if (i == (width - 1) && j == 0) {
				texture_render_clip(source,
						    &box,
						    &frame_top_right,
						    cam);
			} else if (i == 0 && j == (height - 1)) {
				texture_render_clip(source,
						    &box,
						    &frame_bottom_left,
						    cam);
			} else if (i == (width - 1) && j == (height - 1)) {
				texture_render_clip(source,
						    &box,
						    &frame_bottom_right,
						    cam);
			} else if (i == 0) {
				texture_render_clip(source,
						    &box,
						    &frame_left,
						    cam);
			} else if (i == (width - 1)) {
				texture_render_clip(source,
						    &box,
						    &frame_right,
						    cam);
			} else if (j == 0) {
				texture_render_clip(source,
						    &box,
						    &frame_top,
						    cam);
			} else if (j == (height - 1)) {
				texture_render_clip(source,
						    &box,
						    &frame_bottom,
						    cam);
			} else {
				texture_render_clip(source,
						    &box,
						    &frame_center,
						    cam);
			}
		}
	}
	SDL_SetRenderTarget(cam->renderer, NULL);
	return frame;
}
