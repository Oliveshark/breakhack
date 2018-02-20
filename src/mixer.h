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

#ifndef MIXER_H_
#define	MIXER_H_

#include <stdbool.h>

typedef enum Music_t {
	MENU_MUSIC,
	GAME_MUSIC0,
	GAME_MUSIC1,
	GAME_MUSIC2,
	LAST_MUSIC
} Music;

typedef enum Fx_t {
	CLICK,
	SWING0,
	SWING1,
	SWING2,
	SWORD_HIT,
	BONK,
	DEATH,
	COIN,
	BOTTLE,
	BUBBLE0,
	BUBBLE1,
	BUBBLE2,
	EAT,
	LEVEL_UP,
	NEXT_LEVEL,
	SPLAT,
	PLAYER_HIT0,
	PLAYER_HIT1,
	PLAYER_HIT2,
	LAST_EFFECT
} Fx;

void
mixer_init(void);

bool
mixer_toggle_sound(void);

bool
mixer_toggle_music(void);

void
mixer_play_effect(Fx fx);

void
mixer_play_music(Music);

void
mixer_stop_music(void);

void
mixer_close(void);

#endif // MIXER_H_
