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

static Uint8 controller_mode = 0;

static bool
render_button_texture_for(const char *text, Position pos, Camera *cam)
{
	if (!controller_mode) {
		return false;
	}

	Texture *t = texturecache_add("Extras/Controller.png");
	SDL_Rect clip;
	if (strcmp(text, "1") == 0) {
		clip = CONTROLLER_BTN(0, controller_mode);
	} else if (strcmp(text, "2") == 0) {
		clip = CONTROLLER_BTN(16, controller_mode);
	} else if (strcmp(text, "3") == 0) {
		clip = CONTROLLER_BTN(32, controller_mode);
	} else if (strcmp(text, "4") == 0) {
		clip = CONTROLLER_BTN(48, controller_mode);
	} else if (strcmp(text, "5") == 0) {
		clip = CONTROLLER_BUMPER(32, controller_mode);
	} else if (strcmp(text, "ESC") == 0) {
		clip = CONTROLLER_OPT(32, controller_mode);
	} else if (strcmp(text, "ENTER") == 0) {
		clip = CONTROLLER_OPT(0, controller_mode);
	} else if (strcmp(text, "SPACE") == 0) {
		clip = CLIP16(0, 80);
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
				renderBox.x += 16;
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

void
tooltip_set_controller_mode(Uint8 ctrl_mode)
{
	controller_mode = ctrl_mode;
}
