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
							      (Uint32) ((rowCount * 10 + 48)/16),
							      cam);
	sprite->pos.x = 48;
	sprite->pos.y = 96;
	Texture *texture = sprite->textures[0];
	Texture *text = texture_create();
	texture_load_font(text, "GUI/SDS_8x8.ttf", LOG_FONT_SIZE, 0);
	SDL_SetRenderTarget(cam->renderer, texture->texture);
	SDL_Rect renderBox = { 16, 16, 0, 0 };

	while (*content) {
		if (strlen(*content) > 0) {
			texture_load_from_text(text,
					       *content,
					       C_WHITE,
					       C_WHITE,
					       cam->renderer);
			renderBox.w = text->dim.width;
			renderBox.h = text->dim.height;
			texture_render(text, &renderBox, cam);
		}

		renderBox.y += 10;
		content++;
	}
	SDL_SetRenderTarget(cam->renderer, NULL);
	texture_destroy(text);

	return sprite;
}
