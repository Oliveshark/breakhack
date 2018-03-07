#include "projectile.h"
#include "util.h"

Projectile *
projectile_create(void)
{
	Projectile *p = ec_malloc(sizeof(Projectile));
	p->sprite = sprite_create();
	p->velocity = VECTOR2D_NODIR;
	p->alive = true;
	p->lifetime = timer_create();
	timer_start(p->lifetime);
	return p;
}

void
projectile_update(Projectile *p, float deltatime)
{
	p->sprite->pos.x += (int) (p->velocity.x * deltatime);
	p->sprite->pos.y += (int) (p->velocity.y * deltatime);

	if (timer_get_ticks(p->lifetime) > 2000)
		p->alive = false;
}

void
projectile_render(Projectile *p, Camera *cam)
{
	sprite_render(p->sprite, cam);
}

void
projectile_destroy(Projectile *p)
{
	sprite_destroy(p->sprite);
	timer_destroy(p->lifetime);
	free(p);
}