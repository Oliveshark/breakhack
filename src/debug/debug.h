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
#ifdef DEBUG
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "../update_data.h"

/**
 * \brief Utility function for multi purpose testing
 *
 * This function is intended for anything required during testing. This is to
 * avoid manipulating other systems in order to test functionality.
 *
 * \param update_data The UpdateData struct
 */
void
debug_update(UpdateData *update_data);

void
debug_render(Camera *cam);

#endif  // _DEBUG_H_
#endif  // DEBUG

