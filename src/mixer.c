#include <SDL2/SDL_mixer.h>
#include "mixer.h"
#include "util.h"

static Mix_Music *music = NULL;
static Mix_Chunk *effects[LAST_EFFECT];

static Mix_Chunk*
load_effect(char *path)
{
	Mix_Chunk *effect = Mix_LoadWAV(path);
	if (effect == NULL)
		fatal("Failed to load effect: %s", Mix_GetError());
	return effect;
}

static void
load_effects(void)
{
	effects[CLICK] = load_effect("assets/Sounds/FX/click.wav");
}

void
mixer_init(void)
{
	if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) == -1) {
		fatal("Failed to load sound: %s", Mix_GetError());
	}
	load_effects();
	music = Mix_LoadMUS("assets/Sounds/Music/fantasy-forest-battle.ogg");
	if (music == NULL)
		fatal("Failed to load music: %s", Mix_GetError());
}

void
mixer_play_effect(Fx fx)
{
	if (Mix_PlayChannel( -1, effects[fx], 0) == -1)
		error("Unable to play sound: %u", (unsigned int) fx);
}

void
mixer_play_music(void)
{
	if (Mix_PlayingMusic())
		return;

	if (Mix_PlayMusic(music, -1) == -1)
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
	for (size_t i = 0; i < LAST_EFFECT; ++i) {
		Mix_FreeChunk(effects[i]);
	}
	if (music)
		Mix_FreeMusic(music);
	Mix_CloseAudio();
}
