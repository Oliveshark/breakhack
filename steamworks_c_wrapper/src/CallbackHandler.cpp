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
