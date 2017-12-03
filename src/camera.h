#ifndef CAMERA_H_
#define	CAMERA_H_

#include <SDL2/SDL.h>

#include "position.h"

typedef struct {
	Position pos;
	SDL_Renderer *renderer;
} Camera;

Position camera_to_camera_position(Camera *cam, Position *pos);

void camera_follow_position(Camera*, Position*);

#endif // CAMERA_H_
