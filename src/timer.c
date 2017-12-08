#include <SDL2/SDL.h>
#include "util.h"
#include "timer.h"

Timer* timer_create()
{
	Timer *t = ec_malloc(sizeof(Timer));
	t->startTime = 0;
	return t;
}

void timer_start(Timer *t)
{
	t->startTime = SDL_GetTicks();
}

void timer_stop(Timer *t)
{
	t->startTime = 0;
}

bool timer_started(Timer *t)
{
	return t->startTime != 0;
}

unsigned int timer_get_ticks(Timer *t)
{
	if (!t->startTime)
		return 0;

	return SDL_GetTicks() - t->startTime;
}

void timer_destroy(Timer *t)
{
	free(t);
}
