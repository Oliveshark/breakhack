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

#ifndef GAMESTATE_H_
#define	GAMESTATE_H_

typedef enum GameState_t {
	MENU,
	CREDITS,
	SCORE_SCREEN,
	PLAYING,
	IN_GAME_MENU,
	CHARACTER_MENU,
	GAME_SELECT,
	GAME_OVER,
	COMPLETED,
	QUIT
} GameState;

#endif // GAMESTATE_H_
