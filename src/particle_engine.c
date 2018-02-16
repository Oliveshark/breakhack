/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include "particle_engine.h"
#include "linkedlist.h"
#include "util.h"
#include "defines.h"
#include "vector2d.h"
#include "random.h"

typedef struct Particle_t {
	Position pos;
	Vector2d velocity;
	Dimension dim;
	unsigned int movetime;
	unsigned int lifetime;
	SDL_Color color;
} Particle;

typedef struct Engine_t {
	LinkedList *particles;
} Engine;

static Engine		*engine		= NULL;

static void
check_engine(void)
{
	if (!engine)
		fatal("Particle engine not initiated");
}

void
particle_engine_init(void)
{
	if (engine != NULL)
		fatal("Engine already initiated");

	engine = ec_malloc(sizeof(Engine));
	engine->particles = linkedlist_create();
}

void
particle_engine_bloodspray(Position pos, Dimension dim, unsigned int count)
{
	check_engine();

	if (count > 100)
		count = 100;

	for (unsigned int i = 0; i < count; ++i) {
		int x, y, xv, yv, w, h;
		unsigned int mt, lt;
		Particle *p;

		x = get_random(dim.width) + pos.x;
		y = get_random(dim.height) + pos.y;

		xv = get_random(200) - 100;
		yv = get_random(200) - 100;

		mt = get_random(10) + 10;
		lt = get_random(120) + 60;

		w = get_random(3) + 2;
		h = get_random(3) + 2;

		p = ec_malloc(sizeof(Particle));
		p->pos = (Position) { x, y };
		p->velocity = (Vector2d) { xv, yv };
		p->movetime = mt;
		p->lifetime = lt;
		p->dim = (Dimension) { w, h };
		p->color = (SDL_Color) { 255, 0, 0, 255 };
		linkedlist_append(&engine->particles, p);
	}
}

static void
move_particle(Particle *particle, float deltaTime)
{
	if (!particle->movetime)
		return;

	particle->pos.x += particle->velocity.x * deltaTime;
	particle->pos.y += particle->velocity.y * deltaTime;
}

void
particle_engine_update(float deltaTime)
{
	check_engine();
	LinkedList *current, *last;

	current = engine->particles;
	last = NULL;

	while (current) {
		Particle *particle = current->data;

		if (particle->movetime)
			particle->movetime--;

		if (particle->lifetime > 0) {
			particle->lifetime--;
			move_particle(current->data, deltaTime);
			last = current;
			current = current->next;
		} else {
			if (!last) {
				engine->particles = current->next;
				free(current->data);
				free(current);
				current = engine->particles;
			} else {
				last->next = current->next;
				free(current->data);
				free(current);
				current = last->next;
			}
		}
	}
}

static void
render_particle(Particle *p, Camera *cam)
{
	Position pos = camera_to_camera_position(cam, &p->pos);
	SDL_Rect box = { pos.x, pos.y, p->dim.width, p->dim.height };
	SDL_SetRenderDrawColor(cam->renderer,
			       p->color.r,
			       p->color.b,
			       p->color.g,
			       p->color.a);
	SDL_RenderFillRect(cam->renderer, &box);
}

void
particle_engine_render(Camera *cam)
{
	check_engine();
	LinkedList *particles = engine->particles;

	while (particles) {
		render_particle(particles->data, cam);
		particles = particles->next;
	}
}

void
particle_engine_clear(void)
{
	check_engine();
	while (engine->particles)
		free(linkedlist_pop(&engine->particles));
}

void
particle_engine_close(void)
{
	check_engine();

	while (engine->particles)
		free(linkedlist_pop(&engine->particles));

	free(engine);
	engine = NULL;
}
