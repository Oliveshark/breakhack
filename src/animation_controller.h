#ifndef ANIMATION_CONTROLLER_H_
#define ANIMATION_CONTROLLER_H_

#include "camera.h"

enum AnimationType {
	EXPLOSION_ANIMATION,
};

void animation_controller_create(enum AnimationType type, Position pos);

void animation_controller_init(void);

void animation_controller_update(void);

void animation_controller_render(Camera *cam);

void animation_controller_close(void);

#endif // ANIMATION_CONTROLLER_H_
