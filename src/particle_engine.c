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

typedef enum ParticleType {
	RECT,
	LINE
} ParticleType;

typedef struct RectParticle {
	Position pos;
	Dimension dim;
} RectParticle;

typedef struct LineParticle {
	Position startPos;
	Position endPos;
} LineParticle;

typedef union ParticleUnion {
	ParticleType type;
	RectParticle rect;
	LineParticle line;
} ParticleUnion;

typedef struct Particle {
	ParticleType type;
	Vector2d velocity;
	unsigned int movetime;
	unsigned int lifetime;
	bool fixed;
	SDL_Color color;
	Uint8 blend_mode;
	ParticleUnion particle;
} Particle;

typedef struct Engine_t {
	LinkedList *global_particles;
	LinkedList *game_particles;
} Engine;

static Engine		*engine		= NULL;

static Particle*
create_rect_particle(void)
{
	Particle *p = ec_malloc(sizeof(Particle));

	p->type = RECT;
	p->velocity = VECTOR2D_NODIR;
	p->movetime = 100;
	p->lifetime = 100;
	p->fixed = false;
	p->blend_mode = SDL_BLENDMODE_MOD;
	p->color = C_WHITE;
	p->particle.rect.pos = (Position) { 0, 0 };
	p->particle.rect.dim = (Dimension) { 1, 1 };

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
	engine->game_particles = linkedlist_create();
	engine->global_particles = linkedlist_create();
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

		p = create_rect_particle();
		p->particle.rect.pos = (Position) { x, y };
		p->particle.rect.dim = (Dimension) { w, h };
		p->velocity = (Vector2d) { (float) xv, (float) yv };
		p->movetime = mt;
		p->lifetime = lt;
		p->color = C_RED;
		linkedlist_append(&engine->game_particles, p);
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

		p = create_rect_particle();
		p->particle.rect.pos = (Position) { x, y };
		p->particle.rect.dim = (Dimension) { 2, 2 };
		p->velocity = (Vector2d) { (float) xv, (float) yv };
		p->movetime = lt;
		p->lifetime = lt;
		p->color = colors[get_random((unsigned int) c_count-1)];
		linkedlist_append(&engine->game_particles, p);
	}
	free(colors);
}

void
particle_engine_fire_explosion(Position pos, Dimension dim)
{
	check_engine();
	create_explosion(pos, dim, 3, C_YELLOW, C_YELLOW, C_RED);
}

void
particle_engine_eldritch_explosion(Position pos, Dimension dim)
{
	check_engine();
	create_explosion(pos, dim, 1, C_GREEN);
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

		p = create_rect_particle();
		p->velocity = (Vector2d) { 0, 0 };
		p->movetime = lt;
		p->lifetime = lt;
		p->color = color;
		p->particle.rect.pos = (Position) { x, y };
		if (horizontal)
			p->particle.rect.dim = (Dimension) { 20, 1 };
		else
			p->particle.rect.dim = (Dimension) { 2, 20 };
		linkedlist_append(&engine->game_particles, p);
	}
}

void
particle_engine_sparkle(Position pos, Dimension dim, SDL_Color color, bool global)
{
	for (unsigned int i = 0; i < 10; ++i) {
		int x, y, yv, alpha;
		unsigned int lt;
		Particle *p;

		x = get_random(dim.width) + pos.x;
		y = get_random(dim.height) + pos.y;

		alpha = get_random(155) + 100;

		yv = 0 - (get_random(100) + 100);

		lt = get_random(20);

		p = create_rect_particle();
		p->particle.rect.pos = (Position) { x, y };
		p->particle.rect.dim = (Dimension) { 2, 2 };
		p->velocity = (Vector2d) { (float) 0, (float) yv };
		p->movetime = lt;
		p->lifetime = lt;
		p->blend_mode = SDL_BLENDMODE_BLEND;
		p->color = color;
		p->color.a = (Uint8) alpha;
		p->fixed = global;
		if (global)
			linkedlist_append(&engine->global_particles, p);
		else
			linkedlist_append(&engine->game_particles, p);
	}
}

void
particle_engine_wind(Vector2d direction)
{
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
		w = get_random(2) + 2;
		h = get_random(2) + 2;

		velocity = get_random(500) + 500;

		lt = get_random(500);

		p = create_rect_particle();
		p->particle.rect.pos = (Position) { x, y };
		p->particle.rect.dim = (Dimension) { w, h };
		p->velocity = (Vector2d) { direction.x * (float) velocity, direction.y * (float) velocity };
		p->movetime = lt;
		p->lifetime = lt;
		p->color = C_BLUE;
		p->fixed = true;
		linkedlist_append(&engine->game_particles, p);
	}
}

void
particle_engine_heat()
{
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
		int yvel, xvel;

		x = get_random(dim.width) + pos.x;
		y = get_random(dim.height) + pos.y;
		w = get_random(2) + 2;
		h = get_random(2) + 2;

		yvel = get_random(50) - 200;
		xvel = get_random(100) * -((int) get_random(1));

		lt = get_random(500);

		p = create_rect_particle();
		p->particle.rect.pos = (Position) { x, y };
		p->particle.rect.dim = (Dimension) { w, h };
		p->velocity = (Vector2d) { (float) xvel, (float) yvel };
		p->movetime = lt;
		p->lifetime = lt;
		if (get_random(1) == 0)
			p->color = C_YELLOW;
		else
			p->color = C_RED;
		p->fixed = true;
		linkedlist_append(&engine->game_particles, p);
	}
}

static void
move_particle(Particle *particle, float deltaTime)
{
	if (!particle->movetime)
		return;
	if (particle->type == RECT) {
		particle->particle.rect.pos.x +=
			(int) (particle->velocity.x * deltaTime);
		particle->particle.rect.pos.y +=
			(int) (particle->velocity.y * deltaTime);
	} else if (particle->type == LINE) {
		particle->particle.line.startPos.x +=
			(int) (particle->velocity.x * deltaTime);
		particle->particle.line.startPos.y +=
			(int) (particle->velocity.y * deltaTime);
		particle->particle.line.endPos.x +=
			(int) (particle->velocity.x * deltaTime);
		particle->particle.line.endPos.y +=
			(int) (particle->velocity.y * deltaTime);
	}
}

static void
update_particles(LinkedList **particles, float deltaTime)
{
	LinkedList *cleared = linkedlist_create();
	while (*particles) {
		Particle *particle = linkedlist_pop(particles);
		if (particle->movetime)
			particle->movetime--;

		if (particle->lifetime > 0) {
			particle->lifetime--;
			move_particle(particle, deltaTime);
			linkedlist_push(&cleared, particle);
		} else {
			free(particle);
		}
	}

	*particles = cleared;
}

void
particle_engine_update(float deltaTime)
{
	check_engine();

	update_particles(&engine->global_particles, deltaTime);
	update_particles(&engine->game_particles, deltaTime);
}

static void
render_rect_particle(Particle *p, Camera *cam)
{
	Position pos;
	if (p->fixed)
		pos = p->particle.rect.pos;
	else
		pos = camera_to_camera_position(cam, &p->particle.rect.pos);

	SDL_SetRenderDrawBlendMode(cam->renderer, p->blend_mode);

	SDL_Rect box = {
		pos.x,
		pos.y,
		p->particle.rect.dim.width,
		p->particle.rect.dim.height
	};
	SDL_SetRenderDrawColor(cam->renderer,
			       p->color.r,
			       p->color.g,
			       p->color.b,
			       p->color.a);
	SDL_RenderFillRect(cam->renderer, &box);

	// Reset the blend mode
	SDL_SetRenderDrawBlendMode(cam->renderer, SDL_BLENDMODE_BLEND);
}

static void
render_line_particle(Particle *p, Camera *cam)
{
	Position spos, epos;
	if (p->fixed) {
		spos = p->particle.line.startPos;
		epos = p->particle.line.endPos;
	} else {
		spos = camera_to_camera_position(cam,
						 &p->particle.line.startPos);
		epos = camera_to_camera_position(cam,
						 &p->particle.line.endPos);
	}

	SDL_SetRenderDrawBlendMode(cam->renderer, p->blend_mode);
	SDL_SetRenderDrawColor(cam->renderer,
			       p->color.r,
			       p->color.g,
			       p->color.b,
			       p->color.a);
	SDL_RenderDrawLine(cam->renderer, spos.x, spos.y, epos.x, epos.y);

	// Reset the blend mode
	SDL_SetRenderDrawBlendMode(cam->renderer, SDL_BLENDMODE_BLEND);
}

static void
render_particles(LinkedList *particles, Camera *cam)
{
	check_engine();

	LinkedList *render_list = particles;

	while (render_list) {
		Particle *p = render_list->data;
		if (p->type == RECT)
			render_rect_particle(p, cam);
		else
			render_line_particle(p, cam);
		render_list = render_list->next;
	}
}

void
particle_engine_render_game(Camera *cam)
{
	render_particles(engine->game_particles, cam);
}

void
particle_engine_render_global(Camera *cam)
{
	render_particles(engine->global_particles, cam);
}

void
particle_engine_clear(void)
{
	check_engine();
	while (engine->game_particles)
		free(linkedlist_pop(&engine->game_particles));
	while (engine->global_particles)
		free(linkedlist_pop(&engine->global_particles));
}

void
particle_engine_close(void)
{
	particle_engine_clear();
	free(engine);
	engine = NULL;
}
