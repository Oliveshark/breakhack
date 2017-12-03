#include "camera.h"
#include "map.h"

Position camera_to_camera_position(Camera *cam, Position *pos)
{
	return (Position) {
		pos->x - cam->pos.x,
		pos->y - cam->pos.y
	};
}

void camera_follow_position(Camera *cam, Position *pos)
{
	unsigned int room_width, room_height;

	room_width = MAP_ROOM_WIDTH * TILE_DIMENSION;
	room_height = MAP_ROOM_HEIGHT * TILE_DIMENSION;

	if (pos->x <= 0)
		cam->pos.x = 0;
	else
		cam->pos.x = pos->x - (pos->x % room_width);

	if (pos->y <= 0)
		cam->pos.y = 0;
	else
		cam->pos.y = pos->y - (pos->y % room_height);
}
