#include "roommatrix.h"
#include "util.h"
#include "map.h"

RoomMatrix* roommatrix_create()
{
	RoomMatrix *m = ec_malloc(sizeof(RoomMatrix));
	roommatrix_reset(m);
	return m;
}

void roommatrix_populate_from_map(RoomMatrix *rm, Map *m)
{
	int i, j;
	Room *r;

	roommatrix_reset(rm);

	rm->roomPos = m->currentRoom;
	r = m->rooms[rm->roomPos.x][rm->roomPos.y];

	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			if (!r->tiles[i][j])
				continue;
			rm->spaces[i][j].occupied = r->tiles[i][j]->collider;
		}
	}
}

void roommatrix_reset(RoomMatrix *m)
{
	int i, j;

	for (i = 0; i < MAP_ROOM_WIDTH; ++i) {
		for (j = 0; j < MAP_ROOM_HEIGHT; ++j) {
			m->spaces[i][j].occupied = false;
			m->spaces[i][j].character = NULL;
			m->spaces[i][j].player = NULL;
		}
	}
	m->roomPos = (Position) { 0, 0 };
}

void roommatrix_destroy(RoomMatrix *m)
{
	free(m);
}
