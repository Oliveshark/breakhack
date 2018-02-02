#include <stdlib.h>
#include "util.h"
#include "gui_button.h"

GuiButton *
gui_button_create(SDL_Rect area)
{
	GuiButton *button = ec_malloc(sizeof(GuiButton));
	button->area = area;
	button->hover = false;
	return button;
}

void
gui_button_check_pointer(GuiButton *button, Pointer *pointer)
{
	SDL_Rect *r = &button->area;
	Position *p = &pointer->sprite->pos;

	button->hover = r->x <= p->x && r->x + r->w >= p->x &&
		r->y <= p->y && r->y + r->h >= p->y;

	if (button->hover)
		debug("Pointer is over button");
}

void
gui_button_destroy(GuiButton *button)
{
	free(button);
}
