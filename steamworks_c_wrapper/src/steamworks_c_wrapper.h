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

#pragma once

#include <stdint.h>

int64_t
c_SteamAPI_Init(void);

int64_t
c_SteamAPI_GetAppID(void);

bool
c_SteamAPI_RestartAppIfNecessary(void);

void
c_SteamAPI_RunCallbacks(void);

void
c_SteamAPI_SetCallbacks(void(*recvCB)(void), void(*storCB)(void), void(*recvLB)(int64_t, const char *));

bool
c_SteamUserStats_RequestCurrentStats(void);

bool
c_SteamUserStats_SetAchievement(const char *id);

void
c_SteamUserStats_GetAchievement(const char *achId, bool *achieved);

const char *
c_SteamUserStats_GetAchievementDisplayAttribute(const char *achId, const char *attrName);

void
c_SteamUserStats_FindLeaderboard(const char *name);

void
c_SteamUserStats_UploadLeaderboardScore(int64_t hLeaderboard, int32_t nScore, const int32_t *details, int32_t nDetails);

void
c_SteamAPI_Shutdown(void);
