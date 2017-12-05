#include "position.h"
#include "defines.h"

Position position_to_matrix_coords(Position *src)
{
	unsigned int room_px_width, room_px_height;
	Position pos;

	room_px_width = TILE_DIMENSION * MAP_ROOM_WIDTH;
	room_px_height = TILE_DIMENSION * MAP_ROOM_HEIGHT;

	pos.x = (src->x % room_px_width) / TILE_DIMENSION;
	pos.y = (src->y % room_px_height) / TILE_DIMENSION;

	return pos;
}
