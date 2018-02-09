#include <stdlib.h>
#include "util.h"
#include "gui_button.h"
#include "collisions.h"

GuiButton *
gui_button_create(SDL_Rect area, void (*event)(void*), void *usrdata)
{
	GuiButton *button = ec_malloc(sizeof(GuiButton));
	button->area = area;
	button->hover = false;
	button->usrdata = usrdata;
	button->event = event;
	return button;
}

void
gui_button_check_pointer(GuiButton *button, Pointer *pointer)
{
	button->hover = position_in_rect(&pointer->sprite->pos, &button->area);
	pointer_toggle_clickable_pointer(pointer, button->hover);
}

void
gui_button_handle_event(GuiButton *button, SDL_Event *event)
{
	if (event->type == SDL_MOUSEBUTTONDOWN) {

		if (event->button.button != SDL_BUTTON_LEFT)
			return;

		if (button->hover && button->event)
			button->event(button->usrdata);

	} else if (event->type == SDL_MOUSEMOTION) {
		Position p = { event->motion.x, event->motion.y };
		button->hover = position_in_rect(&p, &button->area);
	}
}

void
gui_button_destroy(GuiButton *button)
{
	free(button);
}
