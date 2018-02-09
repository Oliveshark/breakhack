#include "collisions.h"

bool
position_in_rect(Position *p, SDL_Rect *r)
{
	return r->x <= p->x && r->x + r->w >= p->x &&
		r->y <= p->y && r->y + r->h >= p->y;
}
