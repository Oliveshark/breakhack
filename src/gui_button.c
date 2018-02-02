#include <stdlib.h>
#include "util.h"
#include "gui_button.h"

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
	SDL_Rect *r = &button->area;
	Position *p = &pointer->sprite->pos;

	button->hover = r->x <= p->x && r->x + r->w >= p->x &&
		r->y <= p->y && r->y + r->h >= p->y;

	pointer_toggle_clickable_pointer(pointer, button->hover);
}

void
gui_button_handle_event(GuiButton *button, SDL_Event *event)
{
	if (event->type != SDL_MOUSEBUTTONDOWN)
		return;

	if (event->button.button != SDL_BUTTON_LEFT)
		return;

	if (button->hover)
		button->event(button->usrdata);
}

void
gui_button_destroy(GuiButton *button)
{
	free(button);
}
