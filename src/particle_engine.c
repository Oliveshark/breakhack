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
#include <stdarg.h>
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
	bool fixed;
	SDL_Color color;
} Particle;

typedef struct Engine_t {
	LinkedList *particles;
} Engine;

static Engine		*engine		= NULL;

static Particle*
create_particle(void)
{
	Particle *p = ec_malloc(sizeof(Particle));
	p->pos = (Position) { 0, 0 };
	p->dim = (Dimension) { 1, 1 };
	p->velocity = VECTOR2D_NODIR;
	p->movetime = 100;
	p->lifetime = 100;
	p->fixed = false;
	p->color = (SDL_Color) { 255, 255, 255, 255 };
	return p;
}

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

		p = create_particle();
		p->pos = (Position) { x, y };
		p->velocity = (Vector2d) { (float) xv, (float) yv };
		p->movetime = mt;
		p->lifetime = lt;
		p->dim = (Dimension) { w, h };
		p->color = (SDL_Color) { 255, 0, 0, 255 };
		linkedlist_append(&engine->particles, p);
	}
}

static void
create_explosion(Position pos, Dimension dim, unsigned int c_count, ...)
{
	SDL_Color *colors = ec_malloc(c_count * sizeof(SDL_Color));

	va_list(args);
	va_start(args, c_count);
	for (size_t i = 0; i < c_count; ++i) {
		colors[i] = va_arg(args, SDL_Color);
	}
	va_end(args);

	for (unsigned int i = 0; i < 150; ++i) {
		int x, y, xv, yv;
		unsigned int lt;
		Particle *p;

		x = get_random(dim.width) + pos.x;
		y = get_random(dim.height) + pos.y;

		xv = get_random(600) - 300;
		yv = get_random(600) - 300;

		lt = get_random(10);

		p = create_particle();
		p->pos = (Position) { x, y };
		p->velocity = (Vector2d) { (float) xv, (float) yv };
		p->movetime = lt;
		p->lifetime = lt;
		p->dim = (Dimension) { 2, 2 };
		p->color = colors[get_random((unsigned int) c_count-1)];
		linkedlist_append(&engine->particles, p);
	}
	free(colors);
}

void
particle_engine_fire_explosion(Position pos, Dimension dim)
{
	static SDL_Color red = { 255, 0, 0, 255 };
	static SDL_Color yellow = { 255, 255, 0, 255 };

	check_engine();
	create_explosion(pos, dim, 3, yellow, yellow, red);
}

void
particle_engine_eldritch_explosion(Position pos, Dimension dim)
{
	static SDL_Color green = { 0, 255, 0, 255 };

	check_engine();
	create_explosion(pos, dim, 1, green);
}

void
particle_engine_speed_lines(Position pos, Dimension dim, bool horizontal)
{
	static SDL_Color color = { 0, 0, 255, 200 };
	unsigned int count = (unsigned int) (dim.width + dim.height) / 2;

	if (dim.width == 0 || dim.height == 0)
		return;

	for (unsigned int i = 0; i < count; ++i) {
		int x, y;
		unsigned int lt;
		Particle *p;

		x = get_random(dim.width) + pos.x;
		y = get_random(dim.height) + pos.y;

		lt = get_random(10);

		p = create_particle();
		p->pos = (Position) { x, y };
		p->velocity = (Vector2d) { 0, 0 };
		p->movetime = lt;
		p->lifetime = lt;
		if (horizontal)
			p->dim = (Dimension) { 20, 1 };
		else
			p->dim = (Dimension) { 2, 20 };
		p->color = color;
		linkedlist_append(&engine->particles, p);
	}
}

void
particle_engine_wind(Vector2d direction)
{
	static SDL_Color color = { 255, 255, 255, 255 };
	unsigned int count = 5;

	Position pos = { 0, 0 };
	Dimension dim = { GAME_VIEW_WIDTH, GAME_VIEW_HEIGHT };

	if (dim.width == 0 || dim.height == 0)
		return;

	for (unsigned int i = 0; i < count; ++i) {
		int x, y;
		int w, h;
		unsigned int lt;
		Particle *p;
		int velocity;

		x = get_random(dim.width) + pos.x;
		y = get_random(dim.height) + pos.y;
		w = get_random(2) + 1;
		h = get_random(2) + 1;

		velocity = get_random(500) + 500;

		lt = get_random(500);

		p = create_particle();
		p->pos = (Position) { x, y };
		p->velocity = (Vector2d) { direction.x * (float) velocity, direction.y * (float) velocity };
		p->movetime = lt;
		p->lifetime = lt;
		p->dim = (Dimension) { w, h };
		p->color = color;
		p->fixed = true;
		linkedlist_append(&engine->particles, p);
	}
}

static void
move_particle(Particle *particle, float deltaTime)
{
	if (!particle->movetime)
		return;

	particle->pos.x += (int) (particle->velocity.x * deltaTime);
	particle->pos.y += (int) (particle->velocity.y * deltaTime);
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
	Position pos;
	if (p->fixed)
		pos = p->pos;
	else
		pos = camera_to_camera_position(cam, &p->pos);

	SDL_Rect box = { pos.x, pos.y, p->dim.width, p->dim.height };
	SDL_SetRenderDrawColor(cam->renderer,
			       p->color.r,
			       p->color.g,
			       p->color.b,
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
