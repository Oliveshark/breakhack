#ifndef POSITION_H_
#define	POSITION_H_

#include <stdbool.h>

typedef struct {
	int x;
	int y;
} Position;

Position position_to_matrix_coords(Position*);

Position position_to_room_coords(Position*);

bool position_in_room(Position *pos, Position *roomPos);

bool position_equals(const Position*, const Position*);

#endif // POSITION_H_
