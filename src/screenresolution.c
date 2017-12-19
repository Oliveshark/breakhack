#include "defines.h"

#ifndef WINDOWS
#include <X11/Xlib.h>
#endif
#include <stdlib.h>

#include "screenresolution.h"

Dimension getScreenDimensions(void)
{
#ifndef WINDOWS
	Display *d = XOpenDisplay(NULL);
	Screen *s = DefaultScreenOfDisplay(d);
	Dimension dim = (Dimension) { s->width, s->height };

	free(d);
	free(s);
#else
	Dimension dim = (Dimension) { 1920, 1080 };
#endif

	return dim;
}
