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
#include "gamestate.h"

typedef enum Music_t {
	MENU_MUSIC,
	GAME_MUSIC0,
	GAME_MUSIC1,
	GAME_MUSIC2,
	BOSS_MUSIC0,
	LAST_MUSIC
} Music;

typedef enum Fx_t {
	CLICK,
	SWING0,
	SWING1,
	SWING2,
	SWOOSH,
	TRIPPLE_SWING,
	SWORD_HIT,
	DOUBLE_SWORD_HIT,
	TRIPPLE_SWORD_HIT,
	BONK,
	DEATH,
	FALL0,
	FALL1,
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
	DAGGER_PICKUP,
	SLAM,
	MAGIC_PICKUP,
	CHEST_OPEN,
	FADE_IN,
	FADE_OUT,
	BURST,
	DOOR_OPEN,
	KEY_PICKUP,
	BLINK_EFFECT,
	BLAST_EFFECT,
	EXPLOSION_EFFECT,
	LAST_EFFECT
} Fx;

void
mixer_init(void);

bool
mixer_toggle_sound(void);

bool
mixer_toggle_music(GameState*);

void
mixer_play_effect(Fx fx);

void
mixer_play_music(Music);

void
mixer_stop_music(void);

void
mixer_close(void);

#endif // MIXER_H_
