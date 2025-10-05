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

#include <SDL3_mixer/SDL_mixer.h>
#include "mixer.h"
#include "util.h"
#include "io_util.h"
#include "settings.h"
#include "random.h"

#define FX_TRACK_COUNT 8

static MIX_Audio *effects[LAST_EFFECT];

static MIX_Audio *current_song = NULL;
static Music loaded_song = LAST_MUSIC;

static MIX_Mixer *mixer = NULL;
static MIX_Track *music_track = NULL;

static uint8_t fx_track_idx = 0;
static MIX_Track *fx_tracks[FX_TRACK_COUNT] = { NULL };

static SDL_PropertiesID music_track_properties = 0;

static char *music[LAST_MUSIC] = {
	 "Sounds/Music/fantasy-forest-battle.ogg",		  // MENU_MUSIC
	 "Sounds/Music/fantasy-game-background-looping.ogg",	  // GAME_MUSIC0
	 "Sounds/Music/bog-creatures-on-the-move-looping.ogg",	  // GAME_MUSIC1
	 "Sounds/Music/fantascape-looping.ogg",			  // GAME_MUSIC2
	 "Sounds/Music/forward-assault.ogg"			  // BOSS_MUSIC0
};

static MIX_Audio*
load_song(char *path)
{
	MIX_Audio *m = MIX_LoadAudio_IO(mixer, io_load_rwops(path), false, true);
	if (m == NULL)
		fatal("Failed to load music: %s", SDL_GetError());
	return m;
}

static MIX_Audio*
load_effect(char *path)
{
	verbose("Loading effect: %s", path);
	SDL_IOStream *io = io_load_rwops(path);
	verbose("Loaded effect: %s", path);
	MIX_Audio *effect = MIX_LoadAudio_IO(mixer, io, false, true);
	if (effect == NULL)
		fatal("Failed to load effect (%s): %s", path, SDL_GetError());
	return effect;
}

static void
load_effects(void)
{
	effects[BLAST_EFFECT] = load_effect("Sounds/FX/blast.wav");
	effects[BLINK_EFFECT] = load_effect("Sounds/FX/blink.wav");
	effects[BONK] = load_effect("Sounds/FX/bonk.wav");
	effects[BOTTLE] = load_effect("Sounds/FX/bottle.wav");
	effects[BUBBLE0] = load_effect("Sounds/FX/bubble.wav");
	effects[BUBBLE1] = load_effect("Sounds/FX/bubble2.wav");
	effects[BUBBLE2] = load_effect("Sounds/FX/bubble3.wav");
	effects[BURST] = load_effect("Sounds/FX/burst.wav");
	effects[CHEST_OPEN] = load_effect("Sounds/FX/chest_open.wav");
	effects[CLICK] = load_effect("Sounds/FX/click.wav");
	effects[COIN] = load_effect("Sounds/FX/coin.wav");
	effects[COIN_DROP1] = load_effect("Sounds/FX/coin2.wav");
	effects[COIN_DROP2] = load_effect("Sounds/FX/coin3.wav");
	effects[DAGGER_PICKUP] = load_effect("Sounds/FX/dagger_pickup.wav");
	effects[DEATH] = load_effect("Sounds/FX/death.wav");
	effects[DOOR_OPEN] = load_effect("Sounds/FX/door_open.wav");
	effects[DOUBLE_SWORD_HIT] = load_effect("Sounds/FX/double_sword_hit.wav");
	effects[EAT] = load_effect("Sounds/FX/eat.wav");
	effects[EXPLOSION_EFFECT] = load_effect("Sounds/FX/explosion.wav");
	effects[FADE_IN] = load_effect("Sounds/FX/fade_in.wav");
	effects[FADE_OUT] = load_effect("Sounds/FX/fade_out.wav");
	effects[FALL0] = load_effect("Sounds/FX/fall0.wav");
	effects[FALL1] = load_effect("Sounds/FX/fall1.wav");
	effects[FLESH_DROP1] = load_effect("Sounds/FX/flesh_drop1.wav");
	effects[FLESH_DROP2] = load_effect("Sounds/FX/flesh_drop2.wav");
	effects[FREEZE] = load_effect("Sounds/FX/freeze.wav");
	effects[GROWL] = load_effect("Sounds/FX/growl.wav");
	effects[KEY_PICKUP] = load_effect("Sounds/FX/key_pickup.wav");
	effects[LEVEL_UP] = load_effect("Sounds/FX/level_up.wav");
	effects[MAGIC_PICKUP] = load_effect("Sounds/FX/magic_pickup.wav");
	effects[METAL_DROP] = load_effect("Sounds/FX/metal-small3.wav");
	effects[NEXT_LEVEL] = load_effect("Sounds/FX/next_level.wav");
	effects[PLAYER_HIT0] = load_effect("Sounds/FX/fistpunch_vocal_01.wav");
	effects[PLAYER_HIT1] = load_effect("Sounds/FX/fistpunch_vocal_02.wav");
	effects[PLAYER_HIT2] = load_effect("Sounds/FX/fistpunch_vocal_03.wav");
	effects[POTION_DROP] = load_effect("Sounds/FX/potion_drop.wav");
	effects[SLAM] = load_effect("Sounds/FX/slam.wav");
	effects[SPLAT] = load_effect("Sounds/FX/splat.wav");
	effects[SWING0] = load_effect("Sounds/FX/swing.wav");
	effects[SWING1] = load_effect("Sounds/FX/swing2.wav");
	effects[SWING2] = load_effect("Sounds/FX/swing3.wav");
	effects[SWOOSH] = load_effect("Sounds/FX/swoosh.wav");
	effects[SWORD_HIT] = load_effect("Sounds/FX/sword_hit.wav");
	effects[TRIPPLE_SWING] = load_effect("Sounds/FX/tripple_swing.wav");
	effects[TRIPPLE_SWORD_HIT] = load_effect("Sounds/FX/tripple_sword_hit.wav");
}

void
mixer_init(void)
{
	if (!MIX_Init()) {
		fatal("Failed to init SDL_Mixer: %s", SDL_GetError());
	}

	SDL_AudioSpec desired;
	desired.freq = 44100;
	desired.format = SDL_AUDIO_F32;
	desired.channels = 2;

	mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired);
	if (mixer == NULL) {
		fatal("Failed to init mixer: %s", SDL_GetError());
	}

	music_track = MIX_CreateTrack(mixer);
	if (music_track == NULL) {
		fatal("Failed to create music track: %s", SDL_GetError());
	}
	MIX_Track *tmp;
	for (size_t i = 0; i < FX_TRACK_COUNT; ++i) {
		tmp = MIX_CreateTrack(mixer);
		if (tmp == NULL) {
			fatal("Failed to allocate fx track: %s", SDL_GetError());
		}
		fx_tracks[i] = tmp;
	}

	music_track_properties = SDL_CreateProperties();
	if (music_track_properties == 0) {
		fatal("Failed to create music properties: %s", SDL_GetError());
	}
	if (!SDL_SetNumberProperty(music_track_properties, MIX_PROP_PLAY_LOOPS_NUMBER, -1)) {
		fatal("Failed to set loop property: %s", SDL_GetError());
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
mixer_toggle_music(const GameState *state)
{
	Settings *settings = settings_get();
	settings->music_enabled = !settings->music_enabled;

	if (MIX_TrackPlaying(music_track) && !settings->music_enabled) {
		MIX_PauseTrack(music_track);
	} else if (MIX_TrackPaused(music_track)) {
		MIX_ResumeTrack(music_track);
	} else {
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

	fx_track_idx = (uint8_t)((fx_track_idx + 1) % FX_TRACK_COUNT);
	MIX_Track *track = fx_tracks[fx_track_idx];
	MIX_SetTrackAudio(track, effects[fx]);
	if (!MIX_PlayTrack(track, 0))
		error("Unable to play sound: %u", (unsigned int) fx);
}

void
mixer_play_music(Music mus)
{
	/* TODO(Linus): We should be able to leverage this call here:
		-> MIX_SetTrackIOStream(MIX_Track *track, SDL_IOStream *io, bool closeio);
	   This should reduce RAM usage. */
	if (!settings_get()->music_enabled)
		return;

	if (mus != loaded_song) {
		if (current_song)
			MIX_DestroyAudio(current_song);
		current_song = load_song(music[mus]);
		loaded_song = mus;
	}

	if (MIX_TrackPlaying(music_track))
		mixer_stop_music();

	MIX_SetTrackAudio(music_track, current_song);
	if (!MIX_PlayTrack(music_track, music_track_properties))
		fatal("Failed to play music");
}

void
mixer_stop_music(void)
{
	if (MIX_TrackPlaying(music_track))
		MIX_StopTrack(music_track, 0);
}

void
mixer_close(void)
{
	if (music_track) {
		MIX_DestroyTrack(music_track);
	}
	for (size_t i = 0; i < FX_TRACK_COUNT; ++i) {
		if (fx_tracks[i]) {
			MIX_DestroyTrack(fx_tracks[i]);
		}
	}

	if (current_song) {
		MIX_DestroyAudio(current_song);
	}
	for (size_t i = 0; i < LAST_EFFECT; ++i) {
		if (effects[i]) {
			MIX_DestroyAudio(effects[i]);
		}
	}

	if (music_track_properties) {
		SDL_DestroyProperties(music_track_properties);
	}

	if (mixer) {
		MIX_DestroyMixer(mixer);
	}

	MIX_Quit();
}
