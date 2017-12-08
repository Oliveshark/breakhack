#ifndef TIMER_H_
#define	TIMER_H_

#include <stdbool.h>

typedef struct {
	unsigned int startTime;
} Timer;

Timer* timer_create();
void timer_start(Timer*);
void timer_stop(Timer*);
bool timer_started(Timer*);
unsigned int timer_get_ticks(Timer*);
void timer_destroy(Timer*);

#endif // TIMER_H_
