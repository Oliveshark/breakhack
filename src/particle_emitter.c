#include "particle_emitter.h"
#include "util.h"

#include <stdlib.h>

ParticleEmitter *
particle_emitter_create(void)
{
	ParticleEmitter *emitter = ec_malloc(sizeof(ParticleEmitter));
	emitter->pos = POS(0, 0);
	emitter->dim = DIM(32, 32);
	emitter->particle_func = NULL;
	emitter->timer = _timer_create();
	emitter->timestep = 1;
	emitter->enabled = false;
	return emitter;
}

void
particle_emitter_render(ParticleEmitter *emitter)
{
	if (!emitter->enabled)
		return;

	if (!timer_started(emitter->timer)) {
		timer_start(emitter->timer);
		return;
	}

	if (timer_get_ticks(emitter->timer) < emitter->timestep)
		return;

	timer_stop(emitter->timer);
	timer_start(emitter->timer);

	if (emitter->particle_func)
		emitter->particle_func(emitter->pos, emitter->dim);
	else
		error("Particle emitter missing particle_func");
}

void
particle_emitter_update(ParticleEmitter *emitter, Position pos, Dimension dim)
{
	emitter->pos = pos;
	emitter->dim = dim;
}

void
particle_emitter_destroy(ParticleEmitter *emitter)
{
	timer_destroy(emitter->timer);
	emitter->timer = NULL;
	free(emitter);
}
