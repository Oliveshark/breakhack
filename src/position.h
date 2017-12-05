#ifndef POSITION_H_
#define	POSITION_H_

typedef struct {
	int x;
	int y;
} Position;

Position position_to_matrix_coords(Position*);

#endif // POSITION_H_
