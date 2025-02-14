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

#include <steam_api.h>

extern "C" {
#include "steamworks_c_wrapper.h"
}
#include "CallbackHandler.h"

static bool m_Initiated = false;
static int64 m_AppId = 0;
static CallbackHandler *m_CallbackHandler = NULL;

extern "C" int64_t
c_SteamAPI_Init()
{
	if (SteamAPI_Init()) {
		m_AppId = SteamUtils()->GetAppID();
		m_Initiated = true;
	}
	m_CallbackHandler = new CallbackHandler(m_AppId);
	return m_AppId;
}

extern "C" int64_t
c_SteamAPI_GetAppID()
{
	if (!m_Initiated)
		return 0;
	m_AppId = SteamUtils()->GetAppID();
	return m_AppId;
}

bool c_SteamAPI_RestartAppIfNecessary()
{
	return SteamAPI_RestartAppIfNecessary(931040);
}

void
c_SteamAPI_RunCallbacks(void)
{
	if (m_Initiated)
		SteamAPI_RunCallbacks();
}

extern "C" void c_SteamAPI_SetCallbacks(void(*storCB)(void), void(*recvLB)(int64_t, const char *))
{
	m_CallbackHandler->statsStoredCb = storCB;
	m_CallbackHandler->leaderboardReceivedCb = recvLB;
}

extern "C" void
c_SteamAPI_Shutdown()
{
	delete m_CallbackHandler;
	SteamAPI_Shutdown();
}

extern "C" bool
c_SteamUserStats_SetAchievement(const char *pchName)
{
	if (m_CallbackHandler == nullptr)
		return false;

	bool result = SteamUserStats()->SetAchievement(pchName);
	if (result)
		SteamUserStats()->StoreStats();
	return result;
}

extern "C" void
c_SteamUserStats_GetAchievement(const char *achId, bool *achieved)
{
	if (!m_Initiated)
		return;

	SteamUserStats()->GetAchievement(achId, achieved);
}

extern "C" const char*
c_SteamUserStats_GetAchievementDisplayAttribute(const char *achId, const char *attrName)
{
	return SteamUserStats()->GetAchievementDisplayAttribute(achId, attrName);
}

extern "C" void
c_SteamUserStats_FindLeaderboard(const char * name)
{
	if (!m_Initiated || !m_CallbackHandler)
		return;

	SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(name);
	m_CallbackHandler->m_FindLeaderboardCallResult.Set(hSteamAPICall, m_CallbackHandler, &CallbackHandler::OnFindLeaderboard);
}

extern "C" void
c_SteamUserStats_FindOrCreateLeaderboard(const char *name)
{
	if (!m_Initiated || !m_CallbackHandler)
		return;

	SteamAPICall_t hSteamAPICall = SteamUserStats()->FindOrCreateLeaderboard(name, k_ELeaderboardSortMethodDescending, k_ELeaderboardDisplayTypeNumeric);
	m_CallbackHandler->m_FindLeaderboardCallResult.Set(hSteamAPICall, m_CallbackHandler, &CallbackHandler::OnFindLeaderboard);
}

extern "C" void c_SteamUserStats_UploadLeaderboardScore(int64_t hLeaderboard, int32_t nScore, const int32_t *details, int32_t nDetails)
{
	if (!hLeaderboard || !m_Initiated)
		return;

	SteamUserStats()->UploadLeaderboardScore(hLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, nScore, details, nDetails);
}
