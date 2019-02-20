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

#include "camera.h"
#include "map.h"
#include "util.h"
#include "random.h"


// Preserve a link for global access
// See: TODO in camera_shake(void)
static Camera *gCamera = NULL;

Camera *camera_create(SDL_Renderer *renderer)
{
	gCamera = ec_malloc(sizeof(Camera));
	gCamera->renderer = renderer;
	gCamera->shakeTimer = _timer_create();
	gCamera->velocity = VECTOR2D_NODIR;
	gCamera->pos = (Position) { 0, 0 };
	gCamera->basePos = (Position) { 0, 0 };
	return gCamera;
}

Position camera_to_camera_position(Camera *cam, Position *pos)
{
	return (Position) {
		pos->x - cam->pos.x,
		pos->y - cam->pos.y
	};
}

void camera_follow_position(Camera *cam, Position *pos)
{
	int room_width, room_height;
	Position newPos = { 0, 0 };

	room_width = MAP_ROOM_WIDTH * TILE_DIMENSION;
	room_height = MAP_ROOM_HEIGHT * TILE_DIMENSION;

	if (pos->x <= 0)
		newPos.x = 0;
	else if (pos->x >= room_width * MAP_H_ROOM_COUNT)
		newPos.x = room_width * (MAP_H_ROOM_COUNT - 1);
	else
		newPos.x = pos->x - (pos->x % room_width);

	if (pos->y <= 0)
		newPos.y = 0;
	else if (pos->y >= room_height * MAP_V_ROOM_COUNT)
		newPos.y = room_height * (MAP_V_ROOM_COUNT - 1);
	else
		newPos.y = pos->y - (pos->y % room_height);

	cam->basePos = newPos;
	if (!timer_started(cam->shakeTimer))
		cam->pos = newPos;
}

void
camera_destroy(Camera *camera)
{
	timer_destroy(camera->shakeTimer);
	free(camera);
}

void
camera_update(Camera *camera, float deltatime)
{
	if (!timer_started(camera->shakeTimer))
		return;
	if (timer_get_ticks(camera->shakeTimer) > 60) {
		camera->pos = camera->basePos;
		timer_stop(camera->shakeTimer);
		return;
	} else if (timer_get_ticks(camera->shakeTimer) > 20) {
		camera->pos = camera->basePos;
		camera->velocity.x *= -1;
		camera->velocity.y *= -1;
	}
	camera->pos.x += (int) (camera->velocity.x * deltatime);
	camera->pos.y += (int) (camera->velocity.y * deltatime);
}

void
camera_shake(Vector2d dir, int intensity)
{
	// TODO(Linus): This isn't great. If we ever need more cameras this
	// needs to be rethought.
	if (gCamera == NULL)
		return;

	if (timer_started(gCamera->shakeTimer))
		return;
	timer_start(gCamera->shakeTimer);
	gCamera->velocity = dir;
	gCamera->velocity.x *= (float) intensity;
	gCamera->velocity.y *= (float) intensity;
	gCamera->basePos = gCamera->pos;
}
