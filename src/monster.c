#include "monster.h"
#include "util.h"

Monster*
monster_create()
{
	Monster *m = ec_malloc(sizeof(Monster));
	m->sprite = sprite_create();
	return m;
}

void
monster_render(Monster *m, Camera *cam)
{
	sprite_render(m->sprite, cam);
}

void
monster_destroy(Monster *m)
{
	sprite_destroy(m->sprite);
	free(m);
}
