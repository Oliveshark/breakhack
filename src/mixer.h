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
	LAST_EFFECT
} Fx;

void
mixer_init(void);

void
mixer_play_effect(Fx fx);

void
mixer_play_music(Music);

void
mixer_stop_music(void);

void
mixer_close(void);

#endif // MIXER_H_
