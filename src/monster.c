#include "monster.h"
#include "util.h"
#include "player.h"
#include "monster.h"

Monster*
monster_create()
{
	Monster *m = ec_malloc(sizeof(Monster));
	m->sprite = sprite_create();
	m->sprite->clip = (SDL_Rect) { 0, 0, 16, 16 };
	m->stats = (Stats) { 11, 1, 0, 0, 1 };
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
