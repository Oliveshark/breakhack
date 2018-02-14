#ifndef MIXER_H_
#define	MIXER_H_

#include <stdbool.h>

typedef enum Fx_t {
	CLICK,
	LAST_EFFECT
} Fx;

void
mixer_init(void);

void
mixer_play_effect(Fx fx);

void
mixer_play_music(void);

void
mixer_stop_music(void);

void
mixer_close(void);

#endif // MIXER_H_
