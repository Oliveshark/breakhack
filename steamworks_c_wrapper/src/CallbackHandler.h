#pragma once

#include <steam_api.h>

class CallbackHandler
{
private:
	int64 m_AppId = 0;
	bool m_bStatsCallbackReceived = false;

public:
	CallbackHandler(int64 appId);
	STEAM_CALLBACK(CallbackHandler, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(CallbackHandler, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	CCallResult<CallbackHandler, LeaderboardFindResult_t> m_FindLeaderboardCallResult;

	void(*statsReceivedCb)() = nullptr;
	void(*statsStoredCb)() = nullptr;
	void(*leaderboardReceivedCb)(int64, const char*) = nullptr;

	bool CallbackReceived() const;

	void OnFindLeaderboard(LeaderboardFindResult_t *pCallback, bool bIOFailiure);
};