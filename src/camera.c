#include "camera.h"

Position camera_to_camera_position(Camera *cam, Position *pos)
{
	return (Position) {
		pos->x - cam->pos.x,
		pos->y - cam->pos.y
	};
}
