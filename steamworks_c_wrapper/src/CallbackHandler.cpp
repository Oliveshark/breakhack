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

#include "CallbackHandler.h"

CallbackHandler::CallbackHandler(int64 appId) :
	m_CallbackUserStatsReceived(this, &CallbackHandler::OnUserStatsReceived),
	m_CallbackUserStatsStored(this, &CallbackHandler::OnUserStatsStored),
	m_AppId(appId)
{
	// Nothing
}

void
CallbackHandler::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (m_AppId != pCallback->m_nGameID)
		return;

	m_bStatsCallbackReceived = true;
	if (statsReceivedCb && k_EResultOK == pCallback->m_eResult)
		statsReceivedCb();
}

void
CallbackHandler::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	if (m_AppId != pCallback->m_nGameID)
		return;

	if (statsStoredCb && k_EResultOK == pCallback->m_eResult)
		statsStoredCb();
}

bool CallbackHandler::CallbackReceived() const
{
	return m_bStatsCallbackReceived;
}

void CallbackHandler::OnFindLeaderboard(LeaderboardFindResult_t * pCallback, bool bIOFailiure)
{
	if (bIOFailiure || !pCallback->m_bLeaderboardFound)
		return;

	if (leaderboardReceivedCb)
		leaderboardReceivedCb(pCallback->m_hSteamLeaderboard, SteamUserStats()->GetLeaderboardName(pCallback->m_hSteamLeaderboard));
}
