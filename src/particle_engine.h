#ifndef PARTICLE_ENGINE_H_
#define	PARTICLE_ENGINE_H_

#include <SDL2/SDL.h>
#include "position.h"
#include "dimension.h"
#include "camera.h"

void
particle_engine_init(void);

void
particle_engine_bloodspray(Position, Dimension);

void
particle_engine_update(float deltatime);

void
particle_engine_render(Camera*);

void
particle_engine_close(void);

#endif // PARTICLE_ENGINE_H_
