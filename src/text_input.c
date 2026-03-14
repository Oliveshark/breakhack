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

#include "text_input.h"
#include "SDL3/SDL_clipboard.h"
#include "sprite.h"
#include "defines.h"

#define BUF_SIZE 16

static char inputBuffer[BUF_SIZE] = {0};
static Sprite *inputSprite = NULL;
static Sprite *headingSprite = NULL;
static Sprite *hintSprite = NULL;
static SDL_Renderer *gRenderer = NULL;
static bool gInputConfirmed = false;

void
text_input_init(SDL_Window *window, SDL_Renderer *renderer, const char *title, const char *current)
{
	gRenderer = renderer;
	gInputConfirmed = false;

	// Clear and set the buffer
	SDL_strlcpy(inputBuffer, current, BUF_SIZE);

	SDL_StartTextInput(window);

	if (headingSprite == NULL) {
		headingSprite = sprite_create();
		sprite_load_text_texture(headingSprite, "GUI/SDS_8x8.ttf", 0, 18, 1);
		texture_load_from_text(headingSprite->textures[0], title, C_BLUE, C_BLACK, renderer);
		headingSprite->dim = headingSprite->textures[0]->dim;
		headingSprite->pos = (Position){(SCREEN_WIDTH - headingSprite->dim.width) >> 1, SCREEN_HEIGHT / 2 - 40};
		headingSprite->fixed = true;
	}

	if (inputSprite == NULL) {
		inputSprite = sprite_create();
		sprite_load_text_texture(inputSprite, "GUI/SDS_8x8.ttf", 0, 18, 1);
		inputSprite->fixed = true;
	}

	if (hintSprite == NULL) {
		hintSprite = sprite_create();
		sprite_load_text_texture(hintSprite, "GUI/SDS_8x8.ttf", 0, 10, 1);
		texture_load_from_text(hintSprite->textures[0], "ENTER to confirm | ESC to cancel", C_WHITE, C_BLACK,
		                       renderer);
		hintSprite->dim = hintSprite->textures[0]->dim;
		hintSprite->pos = (Position){15, SCREEN_HEIGHT - 25};
		hintSprite->fixed = true;
	}
}

static void
parseBufferToInput(const char *buffer)
{
	for (const char *c = buffer; *c; c++) {
		if (*c >= '0' && *c <= '9') {
			size_t len = SDL_strlen(inputBuffer);
			if (len < sizeof(inputBuffer) - 1) {
				inputBuffer[len] = *c;
				inputBuffer[len + 1] = '\0';
			}
		}
	}
}

void
text_input_update(Input *input)
{
	if (input->textInput[0] != '\0') {
		parseBufferToInput(input->textInput);
	} else if (input_key_is_pressed(input, KEY_BACKSPACE)) {
		size_t len = SDL_strlen(inputBuffer);
		if (len > 0) {
			inputBuffer[len - 1] = '\0';
		}
	} else if (input_key_is_pressed(input, KEY_ENTER)) {
		gInputConfirmed = true;
	} else if (input_modkey_is_pressed(input, KEY_CTRL_V)) {
		char *cb_text = SDL_GetClipboardText();
		parseBufferToInput(cb_text);
		SDL_free(cb_text);
	}
}

bool
text_input_is_confirmed(void)
{
	return gInputConfirmed;
}

const char *
text_input_get_value(void)
{
	return inputBuffer;
}

void
text_input_render(Camera *cam)
{
	SDL_FRect bg = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_SetRenderDrawColor(cam->renderer, 0, 0, 0, 175);
	SDL_RenderFillRect(cam->renderer, &bg);

	sprite_render(headingSprite, cam);

	char displayBuf[20];
	SDL_snprintf(displayBuf, sizeof(displayBuf), "%s_", inputBuffer);
	texture_load_from_text(inputSprite->textures[0], displayBuf, C_YELLOW, C_BLACK, gRenderer);
	inputSprite->dim = inputSprite->textures[0]->dim;
	inputSprite->pos = (Position){(SCREEN_WIDTH - inputSprite->dim.width) >> 1, SCREEN_HEIGHT / 2};
	sprite_render(inputSprite, cam);

	sprite_render(hintSprite, cam);
}

void
text_input_close(SDL_Window *window)
{
	SDL_StopTextInput(window);

	if (headingSprite) {
		sprite_destroy(headingSprite);
		headingSprite = NULL;
	}
	if (inputSprite) {
		sprite_destroy(inputSprite);
		inputSprite = NULL;
	}
	if (hintSprite) {
		sprite_destroy(hintSprite);
		hintSprite = NULL;
	}
}
