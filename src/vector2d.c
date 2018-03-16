#include "vector2d.h"

bool
vector2d_equals(Vector2d v1, Vector2d v2)
{
	return v1.x == v2.x && v1.y == v2.y;
}

bool
vector2d_is_opposite(Vector2d v1, Vector2d v2)
{
	return ((v1.x > 0 && v2.x < 0) ^ (v1.y > 0 && v2.y < 0))
		|| ((v1.x < 0 && v2.x > 0) ^ (v1.y < 0 && v2.y > 0));
}
