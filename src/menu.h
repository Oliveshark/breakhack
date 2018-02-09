#ifndef MENU_H_
#define	MENU_H_

#include <SDL2/SDL.h>
#include "camera.h"
#include "linkedlist.h"
#include "texture.h"
#include "sprite.h"

typedef struct Menu_t {
	LinkedList *items;
	int selected;
} Menu;

Menu *
menu_create(void);

void
menu_handle_event(Menu*, SDL_Event*);

void
menu_item_add(Menu*, Sprite*, Sprite*, void (*)(void*));

void
menu_render(Menu*, Camera*);

void
menu_destroy(Menu*);

#endif // MENU_H_
