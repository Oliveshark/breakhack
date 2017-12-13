#include "monster.h"
#include "util.h"

Monster*
monster_create()
{
	Monster *m = ec_malloc(sizeof(Monster));
	m->sprite = sprite_create();
	m->clip = (SDL_Rect) { 0, 0, 16, 16 };
	return m;
}

void
monster_render(Monster *m, Camera *cam)
{
	m->sprite->textures[0]->clip = m->clip;
	m->sprite->textures[1]->clip = m->clip;
	sprite_render(m->sprite, cam);
}

void
monster_destroy(Monster *m)
{
	sprite_destroy(m->sprite);
	free(m);
}
