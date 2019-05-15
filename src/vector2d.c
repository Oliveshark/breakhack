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

void
vector2d_reverse(Vector2d *vec)
{
	vec->x *= -1;
	vec->y *= -1;
}

Vector2d
vector2d_to_direction(const Vector2d *vec)
{
	Vector2d new = VEC2D(vec->x, vec->y);

	if (new.x > 0)
		new.x = 1;
	else if (new.x == 0)
		new.x = 0;
	else
		new.x = -1;

	if (new.y > 0)
		new.y = 1;
	else if (new.y == 0)
		new.y = 0;
	else
		new.y = -1;

	return new;
}
