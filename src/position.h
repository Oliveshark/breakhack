/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2025  Linus Probert <linus.probert@gmail.com>
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

#ifndef POSITION_H_
#define POSITION_H_

#include <stdbool.h>

#define POS(x, y)                                                                                                      \
	(Position)                                                                                                         \
	{                                                                                                                  \
		x, y                                                                                                           \
	}

/**
 * \brief A 2D position
 */
typedef struct {
	int x; /**< x coordinate */
	int y; /**< y coordinate */
} Position;

/*
 * \brief Return a matrix coord position for a given position
 *
 * \param p The position
 * \return The matrix coord position
 */
Position position_to_matrix_coords(const Position *p);

/*
 * \brief Get the room coord for the room containing the given position
 *
 * \param p The position
 * \return The room coord
 */
Position position_to_room_coords(Position *p);

/*
 * \brief Check is a position is within a given room
 *
 * \param pos The position to check
 * \param roomPos The position of the room
 * \return True if the position is within the room
 */
bool position_in_room(Position *pos, Position *roomPos);

/*
 * \brief Check if two positions are equal
 *
 * \param p1 The first position
 * \param p2 The second position
 * \return True if the two positions are equal
 */
bool position_equals(const Position *p1, const Position *p2);

/*
 * \brief Check if two positions are separated by equal or less of the provided
 * distance.
 *
 * \param distance The maximum distance between the two positions
 * \param p1 The first position
 * \param p2 The second position
 * \return True if the two positions are within the provided distance
 */
bool position_proximity(unsigned int distance, const Position *p1, const Position *p2);

/*
 * \brief Check if a matrix coord is still within the matrix
 *
 * \param p The position to check
 * \return True if the position is within the matrix
 */
bool position_in_roommatrix(const Position *p);

/*
 * \brief Get the tile position for the given world position
 *
 * \param p The world position
 * \return The tile position
 */
Position position_to_tile_pos(const Position *p);

/*
 * \brief Add two positions
 *
 * \param p1 The first position
 * \param p2 The second position
 */
Position position_add(const Position *p1, const Position *p2);

/**
 * \brief Get the world position from matrix coord
 */
Position position_mcord_to_world_pos(const Position *cord, const Position *roomPos);

#endif // POSITION_H_
