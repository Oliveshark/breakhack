#include "defines.h"

#ifndef _WIN32
#include <X11/Xlib.h>
#else // _WIN32
#include <windows.h>
#endif // _WIN32
#include <stdlib.h>

#include "screenresolution.h"

Dimension getScreenDimensions(void)
{
#ifndef _WIN32
	Display *d = XOpenDisplay(NULL);
	Screen *s = DefaultScreenOfDisplay(d);
	Dimension dim = (Dimension) { s->width, s->height };

	free(d);
	free(s);
#else // _WIN32
	Dimension dim = (Dimension) { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
#endif // _WIN32

	return dim;
}
