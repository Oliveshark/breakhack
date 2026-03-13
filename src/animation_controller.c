#include "animation_controller.h"
#include "animation.h"
#include "dimension.h"
#include "linkedlist.h"
#include "sprite.h"
#include "texturecache.h"
#include "util.h"
#include <stdlib.h>

struct {
	LinkedList *animations;
} s_controller;

static void
create_explosion(Position pos)
{
	Animation *a = animation_create(8);
	linkedlist_append(&s_controller.animations, a);

	Texture *t = texturecache_add("Extras/Explosion.png");
	sprite_set_texture(a->sprite, t, 0);
	a->sprite->pos.x = pos.x - 16;
	a->sprite->pos.y = pos.y - 16;
	a->sprite->dim = (Dimension){64, 64};
	a->sprite->clip = (SDL_Rect){0, 0, 32, 32};
	a->sprite->rotationPoint = (SDL_Point){32, 32};

	a->loop = false;
	a->running = true;
	animation_set_frames(a, (AnimationClip[]){{0, 0, 32, 32, 100},
	                                          {32, 0, 32, 32, 100},
	                                          {64, 0, 32, 32, 100},
	                                          {96, 0, 32, 32, 100},
	                                          {128, 0, 32, 32, 100},
	                                          {160, 0, 32, 32, 100},
	                                          {192, 0, 32, 32, 300},
	                                          {224, 0, 32, 32, 300}});
}

void
animation_controller_create(enum AnimationType type, Position pos)
{
	switch (type) {
		case EXPLOSION_ANIMATION:
			create_explosion(pos);
			break;
		default:
			fatal("Invalid animation type: %d", type);
			break;
	}
}

void
animation_controller_init(void)
{
	s_controller.animations = NULL;
}

void
animation_controller_update(void)
{
	LinkedList *node = s_controller.animations;
	LinkedList *prev = NULL;
	while (node != NULL) {
		Animation *a = node->data;
		if (!a->running) {
			if (prev) {
				prev->next = node->next;
			} else {
				s_controller.animations = node->next;
			}
			LinkedList *tmp = node;
			node = node->next;
			animation_destroy(tmp->data);
			free(tmp);
			continue;
		}
		animation_update(a);
		prev = node;
		node = node->next;
	}
}

void
animation_controller_render(Camera *cam)
{
	LinkedList *node = s_controller.animations;
	while (node) {
		animation_render(node->data, cam);
		node = node->next;
	}
}

void
animation_controller_close(void)
{
	while (s_controller.animations) {
		animation_destroy(linkedlist_pop(&s_controller.animations));
	}
}
