#ifndef GUI_BUTTON_H_
#define	GUI_BUTTON_H_

#include <SDL2/SDL.h>
#include "pointer.h"
#include "sprite.h"
#include "linkedlist.h"
#include "camera.h"

typedef struct GuiButton_t {
	SDL_Rect area;
	bool hover;
} GuiButton;

GuiButton *
gui_button_create(SDL_Rect);

void
gui_button_check_pointer(GuiButton*, Pointer*);

void
gui_button_destroy(GuiButton*);

#endif // GUI_BUTTON_H_
