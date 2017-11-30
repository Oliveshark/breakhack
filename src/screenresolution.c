#include <X11/Xlib.h>
#include <stdlib.h>

#include "screenresolution.h"

Dimension getScreenDimensions()
{
	Display *d = XOpenDisplay(NULL);
	Screen *s = DefaultScreenOfDisplay(d);
	Dimension dim = (Dimension) { s->width, s->height };

	free(d);
	free(s);

	return dim;
}
