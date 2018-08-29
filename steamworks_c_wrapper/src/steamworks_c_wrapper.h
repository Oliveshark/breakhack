#pragma once

#include <stdint.h>

int64_t
c_SteamAPI_Init(void);

int64_t
c_SteamAPI_GetAppID(void);

void
c_SteamAPI_RunCallbacks(void);

void
c_SteamAPI_SetCallbacks(void(*)(void), void(*)(void), void(*)(int64_t));

bool
c_SteamUserStats_RequestCurrentStats();

bool
c_SteamUserStats_SetAchievement(const char *id);

void
c_SteamUserStats_GetAchievement(const char *achId, bool *achieved);

const char *
c_SteamUserStats_GetAchievementDisplayAttribute(const char *achId, const char *attrName);

void
c_SteamUserStats_FindLeaderboard(const char *name);

void
c_SteamUserStats_UploadLeaderboardScore(int64_t hLeaderboard, int32_t nScore);

void
c_SteamAPI_Shutdown();

#ifdef __cplusplus
class CallbackHandler
{
public:
	CallbackHandler();
	STEAM_CALLBACK(CallbackHandler, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(CallbackHandler, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	CCallResult<CallbackHandler, LeaderboardFindResult_t> m_FindLeaderboardCallResult;

	void OnFindLeaderboard(LeaderboardFindResult_t *pCallback, bool bIOFailiure);
};
#endif // __cplusplus