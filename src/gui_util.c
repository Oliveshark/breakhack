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

#include "texturecache.h"
#include "gui_util.h"

static Sprite *
render_frame_on_texture(Uint32 width,
			Uint32 height,
			Position offset,
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

	SDL_SetRenderTarget(cam->renderer, texture->texture);
	SDL_RenderClear(cam->renderer);

	SDL_Rect frame_top_left		= CLIP16(offset.x, offset.y);
	SDL_Rect frame_top_right	= CLIP16(offset.x + 32, offset.y);
	SDL_Rect frame_bottom_left	= CLIP16(offset.x, offset.y + 32);
	SDL_Rect frame_bottom_right	= CLIP16(offset.x + 32, offset.y + 32);
	SDL_Rect frame_top		= CLIP16(offset.x + 16, offset.y);
	SDL_Rect frame_bottom		= CLIP16(offset.x + 16, offset.y + 32);
	SDL_Rect frame_center		= CLIP16(offset.x + 16, offset.y + 16);
	SDL_Rect frame_left		= CLIP16(offset.x, offset.y + 16);
	SDL_Rect frame_right		= CLIP16(offset.x + 32, offset.y + 16);

	Texture *source = texturecache_get("GUI/GUI0.png");
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

Sprite *
gui_util_create_frame_sprite(Uint32 width,
			     Uint32 height,
			     Camera *cam)
{
	return render_frame_on_texture(width, height, POS(16, 16*10), cam);
}

Sprite *
gui_util_create_tooltip_frame_sprite(Uint32 width,
				     Uint32 height,
				     Camera *cam)
{
	return render_frame_on_texture(width, height, POS(16*13, 16*13), cam);
}
