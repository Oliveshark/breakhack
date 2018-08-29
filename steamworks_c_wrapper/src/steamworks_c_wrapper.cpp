#include <steam_api.h>

extern "C" {
#include "steamworks_c_wrapper.h"
}

static bool m_Initiated = false;
static int64 m_AppId = 0;
static CallbackHandler *m_CallbackHandler = NULL;
static bool m_RecvCB = false;

static void(*statsReceivedCb)(void) = NULL;
static void(*statsStoredCb)(void) = NULL;
static void(*leaderBoardReceived)(int64) = NULL;


extern "C" int64
c_SteamAPI_Init()
{
	if (SteamAPI_Init()) {
		m_AppId = SteamUtils()->GetAppID();
		m_CallbackHandler = new CallbackHandler();
		m_Initiated = true;
		return m_AppId;
	}
	return 0;
}

extern "C" int64
c_SteamAPI_GetAppID()
{
	if (!m_Initiated)
		return 0;
	m_AppId = SteamUtils()->GetAppID();
	return m_AppId;
}

void
c_SteamAPI_RunCallbacks(void)
{
	if (m_Initiated)
		SteamAPI_RunCallbacks();
}

extern "C" void c_SteamAPI_SetCallbacks(void(*recvCB)(void), void(*storCB)(void), void(*recvLB)(int64_t))
{
	statsReceivedCb = recvCB;
	statsStoredCb = storCB;
	leaderBoardReceived = recvLB;
}

extern "C" void
c_SteamAPI_Shutdown()
{
	delete m_CallbackHandler;
	SteamAPI_Shutdown();
}

extern "C" bool
c_SteamUserStats_RequestCurrentStats()
{
	if (NULL == SteamUserStats() || NULL == SteamUser())
		return false;
	if (!SteamUser()->BLoggedOn())
		return false;

	return SteamUserStats()->RequestCurrentStats();
}

extern "C" bool
c_SteamUserStats_SetAchievement(const char *pchName)
{
	if (!m_RecvCB)
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
	if (!m_Initiated)
		return;

	SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(name);
	m_CallbackHandler->m_FindLeaderboardCallResult.Set(hSteamAPICall, m_CallbackHandler, &CallbackHandler::OnFindLeaderboard);
}

extern "C" void c_SteamUserStats_UploadLeaderboardScore(int64_t hLeaderboard, int32 nScore)
{
	if (!hLeaderboard || !m_Initiated)
		return;

	SteamUserStats()->UploadLeaderboardScore(hLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, nScore, nullptr, 0);
}

CallbackHandler::CallbackHandler() :
	m_CallbackUserStatsReceived(this, &CallbackHandler::OnUserStatsReceived),
	m_CallbackUserStatsStored(this, &CallbackHandler::OnUserStatsStored)
{
	// Nothing
}

void
CallbackHandler::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (m_AppId != pCallback->m_nGameID)
		return;
	m_RecvCB = true;
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

void CallbackHandler::OnFindLeaderboard(LeaderboardFindResult_t * pCallback, bool bIOFailiure)
{
	if (bIOFailiure || !pCallback->m_bLeaderboardFound)
		return;

	if (leaderBoardReceived)
		leaderBoardReceived(pCallback->m_hSteamLeaderboard);
}
