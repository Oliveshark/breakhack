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
