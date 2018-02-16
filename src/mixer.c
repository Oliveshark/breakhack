#include <SDL2/SDL_mixer.h>
#include "mixer.h"
#include "util.h"

static Mix_Music *music[LAST_MUSIC];
static Mix_Chunk *effects[LAST_EFFECT];

static Mix_Music*
load_song(char *path)
{
	Mix_Music *m = Mix_LoadMUS(path);
	if (m == NULL)
		fatal("Failed to load music: %s", Mix_GetError());
	return m;
}

static void
load_music(void)
{
	music[GAME_MUSIC0] = load_song("assets/Sounds/Music/fantasy-game-background-looping.ogg");
	music[GAME_MUSIC1] = load_song("assets/Sounds/Music/bog-creatures-on-the-move-looping.ogg");
	music[GAME_MUSIC2] = load_song("assets/Sounds/Music/fantascape-looping.ogg");

	music[MENU_MUSIC] = load_song("assets/Sounds/Music/fantasy-forest-battle.ogg");
}

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
	effects[SWING0] = load_effect("assets/Sounds/FX/swing.wav");
	effects[SWING1] = load_effect("assets/Sounds/FX/swing2.wav");
	effects[SWING2] = load_effect("assets/Sounds/FX/swing3.wav");
	effects[SWORD_HIT] = load_effect("assets/Sounds/FX/sword_hit.wav");
	effects[BONK] = load_effect("assets/Sounds/FX/bonk.wav");
	effects[DEATH] = load_effect("assets/Sounds/FX/death.wav");
	effects[COIN] = load_effect("assets/Sounds/FX/coin.wav");
	effects[BOTTLE] = load_effect("assets/Sounds/FX/bottle.wav");
	effects[BUBBLE0] = load_effect("assets/Sounds/FX/bubble.wav");
	effects[BUBBLE1] = load_effect("assets/Sounds/FX/bubble2.wav");
	effects[BUBBLE2] = load_effect("assets/Sounds/FX/bubble3.wav");
	effects[EAT] = load_effect("assets/Sounds/FX/eat.wav");
	effects[LEVEL_UP] = load_effect("assets/Sounds/FX/level_up.wav");
	effects[NEXT_LEVEL] = load_effect("assets/Sounds/FX/next_level.wav");
	effects[SPLAT] = load_effect("assets/Sounds/FX/splat.wav");
	effects[PLAYER_HIT0] = load_effect("assets/Sounds/FX/fistpunch_vocal_01.wav");
	effects[PLAYER_HIT1] = load_effect("assets/Sounds/FX/fistpunch_vocal_02.wav");
	effects[PLAYER_HIT2] = load_effect("assets/Sounds/FX/fistpunch_vocal_03.wav");
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

void
mixer_play_effect(Fx fx)
{
	if (Mix_PlayChannel( -1, effects[fx], 0) == -1)
		error("Unable to play sound: %u", (unsigned int) fx);
}

void
mixer_play_music(Music mus)
{
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
