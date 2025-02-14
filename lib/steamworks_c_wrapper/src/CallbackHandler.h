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

#include <steam_api.h>
#include <cstdint>

class CallbackHandler
{
private:
	int64 m_AppId = 0;
	bool m_bStatsCallbackReceived = false;

public:
	CallbackHandler(int64 appId);
	STEAM_CALLBACK(CallbackHandler, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	CCallResult<CallbackHandler, LeaderboardFindResult_t> m_FindLeaderboardCallResult;

	void(*statsStoredCb)() = nullptr;
	void(*leaderboardReceivedCb)(int64_t, const char*) = nullptr;

	bool CallbackReceived() const;

	void OnFindLeaderboard(LeaderboardFindResult_t *pCallback, bool bIOFailiure);
};
