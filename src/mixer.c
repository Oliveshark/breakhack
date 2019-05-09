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
#include "settings.h"
#include "random.h"

static Mix_Chunk *effects[LAST_EFFECT];
static Mix_Music *current_song = NULL;
static Music loaded_song = LAST_MUSIC;

static char *music[LAST_MUSIC] = {
	 "Sounds/Music/fantasy-forest-battle.ogg",		  // MENU_MUSIC
	 "Sounds/Music/fantasy-game-background-looping.ogg",	  // GAME_MUSIC0
	 "Sounds/Music/bog-creatures-on-the-move-looping.ogg",	  // GAME_MUSIC1
	 "Sounds/Music/fantascape-looping.ogg",			  // GAME_MUSIC2
	 "Sounds/Music/forward-assault.ogg"			  // BOSS_MUSIC0
};

static Mix_Music*
load_song(char *path)
{
	Mix_Music *m = Mix_LoadMUS_RW(io_load_rwops(path), true);
	if (m == NULL)
		fatal("Failed to load music: %s", Mix_GetError());
	return m;
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
	effects[SWOOSH] = load_effect("Sounds/FX/swoosh.wav");
	effects[TRIPPLE_SWING] = load_effect("Sounds/FX/tripple_swing.wav");
	effects[SWORD_HIT] = load_effect("Sounds/FX/sword_hit.wav");
	effects[DOUBLE_SWORD_HIT] = load_effect("Sounds/FX/double_sword_hit.wav");
	effects[TRIPPLE_SWORD_HIT] = load_effect("Sounds/FX/tripple_sword_hit.wav");
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
	effects[DAGGER_PICKUP] = load_effect("Sounds/FX/dagger_pickup.wav");
	effects[FALL0] = load_effect("Sounds/FX/fall0.wav");
	effects[FALL1] = load_effect("Sounds/FX/fall1.wav");
	effects[SLAM] = load_effect("Sounds/FX/slam.wav");
	effects[MAGIC_PICKUP] = load_effect("Sounds/FX/magic_pickup.wav");
	effects[CHEST_OPEN] = load_effect("Sounds/FX/chest_open.wav");
	effects[FADE_IN] = load_effect("Sounds/FX/fade_in.wav");
	effects[FADE_OUT] = load_effect("Sounds/FX/fade_out.wav");
	effects[BURST] = load_effect("Sounds/FX/burst.wav");
	effects[DOOR_OPEN] = load_effect("Sounds/FX/door_open.wav");
	effects[KEY_PICKUP] = load_effect("Sounds/FX/key_pickup.wav");
	effects[BLINK_EFFECT] = load_effect("Sounds/FX/blink.wav");
}

void
mixer_init(void)
{
	if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) == -1) {
		fatal("Failed to load sound: %s", Mix_GetError());
	}

	load_effects();
}

bool
mixer_toggle_sound(void)
{
	Settings *settings = settings_get();
	settings->sound_enabled = !settings->sound_enabled;
	return settings->sound_enabled;
}

bool
mixer_toggle_music(GameState *state)
{
	Settings *settings = settings_get();
	settings->music_enabled = !settings->music_enabled;

	if (Mix_PlayingMusic() && !settings->music_enabled)
		Mix_PauseMusic();
	else if (Mix_PausedMusic())
		Mix_ResumeMusic();
	else {
		if (*state == MENU)
			mixer_play_music(MENU_MUSIC);
		else
			mixer_play_music(GAME_MUSIC0 + get_random(2));
	}

	return settings->music_enabled;
}

void
mixer_play_effect(Fx fx)
{
	if (!settings_get()->sound_enabled)
		return;

	if (Mix_PlayChannel( -1, effects[fx], 0) == -1)
		error("Unable to play sound: %u", (unsigned int) fx);
}

void
mixer_play_music(Music mus)
{
	if (!settings_get()->music_enabled)
		return;

	if (mus != loaded_song) {
		if (current_song)
			Mix_FreeMusic(current_song);
		current_song = load_song(music[mus]);
		loaded_song = mus;
	}

	if (Mix_PlayingMusic())
		mixer_stop_music();

	if (Mix_PlayMusic(current_song, -1) == -1)
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
	if (current_song)
		Mix_FreeMusic(current_song);

	Mix_CloseAudio();
}
