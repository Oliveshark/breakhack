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

#include <string.h>
#include "tooltip.h"
#include "gui_util.h"
#include "defines.h"
#include "gui.h"
#include "texturecache.h"

static bool
render_button_texture_for(const char *text, Position pos, Camera *cam)
{
	Texture *t = texturecache_add("Characters/Player0.png");
	SDL_Rect clip = CLIP16(0, 0);
	if (strcmp(text, "1") == 0) {
		// no op
	} else if (strcmp(text, "2") == 0) {
		clip = CLIP16(16, 0);
	} else {
		return false;
	}

	SDL_Rect renderBox = { pos.x, pos.y, 16, 16 };
	texture_render_clip(t, &renderBox, &clip, cam);
	return true;
}

static void
load_texture_for(Texture *text,
		 char *content,
		 SDL_Rect *renderBox,
		 SDL_Renderer *renderer)
{
	texture_load_from_text(text,
			       content,
			       C_WHITE,
			       C_WHITE,
			       renderer);

	renderBox->w = text->dim.width;
	renderBox->h = text->dim.height;
}

Sprite *
tooltip_create(char **content, Camera *cam)
{
	int rowCount = 0;
	char **contentIndex = content;
	while (*contentIndex) {
		rowCount++;
		contentIndex++;
	}

	Sprite *sprite = gui_util_create_tooltip_frame_sprite(BOTTOM_GUI_WIDTH/16 - 6,
							      (Uint32) ((rowCount * 9 + 48)/16),
							      cam);
	sprite->pos.x = 48;
	sprite->pos.y = 48;
	Texture *texture = sprite->textures[0];
	Texture *text = texture_create();
	texture_load_font(text, "GUI/SDS_8x8.ttf", LOG_FONT_SIZE, 0);
	SDL_SetRenderTarget(cam->renderer, texture->texture);
	SDL_Rect renderBox = { 16, 16, 0, 0 };

	while (*content) {
		if (strlen(*content) > 0) {
			if (render_button_texture_for(*content, POS(renderBox.x, renderBox.y - 4), cam)) {
				renderBox.x += 24;
			} else {
				load_texture_for(text, *content, &renderBox, cam->renderer);
				texture_render(text, &renderBox, cam);
				renderBox.x += text->dim.width;
			} 
		} else {
			renderBox.x = 16;
			renderBox.y += 14;
		}
		content++;
	}
	SDL_SetRenderTarget(cam->renderer, NULL);
	texture_destroy(text);

	return sprite;
}
