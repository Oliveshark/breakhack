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

#include <SDL_mixer.h>
#include "mixer.h"
#include "util.h"
#include "io_util.h"

static Mix_Music *music[LAST_MUSIC];
static Mix_Chunk *effects[LAST_EFFECT];

static bool sound_enabled = true;
static bool music_enabled = true;

static Mix_Music*
load_song(char *path)
{
	Mix_Music *m = Mix_LoadMUS_RW(io_load_rwops(path), true);
	if (m == NULL)
		fatal("Failed to load music: %s", Mix_GetError());
	return m;
}

static void
load_music(void)
{
	music[GAME_MUSIC0] = load_song("Sounds/Music/fantasy-game-background-looping.ogg");
	music[GAME_MUSIC1] = load_song("Sounds/Music/bog-creatures-on-the-move-looping.ogg");
	music[GAME_MUSIC2] = load_song("Sounds/Music/fantascape-looping.ogg");

	music[MENU_MUSIC] = load_song("Sounds/Music/fantasy-forest-battle.ogg");
}

static Mix_Chunk*
load_effect(char *path)
{
	Mix_Chunk *effect = Mix_LoadWAV_RW(io_load_rwops(path), true);
	if (effect == NULL)
		fatal("Failed to load effect: %s", Mix_GetError());
	return effect;
}

static void
load_effects(void)
{
	effects[CLICK] = load_effect("Sounds/FX/click.wav");
	effects[SWING0] = load_effect("Sounds/FX/swing.wav");
	effects[SWING1] = load_effect("Sounds/FX/swing2.wav");
	effects[SWING2] = load_effect("Sounds/FX/swing3.wav");
	effects[SWORD_HIT] = load_effect("Sounds/FX/sword_hit.wav");
	effects[BONK] = load_effect("Sounds/FX/bonk.wav");
	effects[DEATH] = load_effect("Sounds/FX/death.wav");
	effects[COIN] = load_effect("Sounds/FX/coin.wav");
	effects[BOTTLE] = load_effect("Sounds/FX/bottle.wav");
	effects[BUBBLE0] = load_effect("Sounds/FX/bubble.wav");
	effects[BUBBLE1] = load_effect("Sounds/FX/bubble2.wav");
	effects[BUBBLE2] = load_effect("Sounds/FX/bubble3.wav");
	effects[EAT] = load_effect("Sounds/FX/eat.wav");
	effects[LEVEL_UP] = load_effect("Sounds/FX/level_up.wav");
	effects[NEXT_LEVEL] = load_effect("Sounds/FX/next_level.wav");
	effects[SPLAT] = load_effect("Sounds/FX/splat.wav");
	effects[PLAYER_HIT0] = load_effect("Sounds/FX/fistpunch_vocal_01.wav");
	effects[PLAYER_HIT1] = load_effect("Sounds/FX/fistpunch_vocal_02.wav");
	effects[PLAYER_HIT2] = load_effect("Sounds/FX/fistpunch_vocal_03.wav");
}

void
mixer_init(void)
{
	if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) == -1) {
		fatal("Failed to load sound: %s", Mix_GetError());
	}
	load_effects();
	load_music();
}

bool
mixer_toggle_sound(void)
{
	sound_enabled = !sound_enabled;
	return sound_enabled;
}

bool
mixer_toggle_music(void)
{
	music_enabled = !music_enabled;

	if (Mix_PlayingMusic() && !music_enabled)
		Mix_PauseMusic();
	else if (Mix_PausedMusic())
		Mix_ResumeMusic();

	return music_enabled;
}

void
mixer_play_effect(Fx fx)
{
	if (!sound_enabled)
		return;

	if (Mix_PlayChannel( -1, effects[fx], 0) == -1)
		error("Unable to play sound: %u", (unsigned int) fx);
}

void
mixer_play_music(Music mus)
{
	if (!music_enabled)
		return;

	if (Mix_PlayingMusic())
		mixer_stop_music();

	if (Mix_PlayMusic(music[mus], -1) == -1)
		fatal("Failed to play music");
}

void
mixer_stop_music(void)
{
	if (Mix_PlayingMusic())
		Mix_HaltMusic();
}

void
mixer_close(void)
{
	for (size_t i = 0; i < LAST_EFFECT; ++i)
		Mix_FreeChunk(effects[i]);
	for (size_t i = 0; i < LAST_MUSIC; ++i)
		Mix_FreeMusic(music[i]);
	Mix_CloseAudio();
}
