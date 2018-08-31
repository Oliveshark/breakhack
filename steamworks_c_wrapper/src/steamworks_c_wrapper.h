#pragma once

#include <stdint.h>

int64_t
c_SteamAPI_Init(void);

int64_t
c_SteamAPI_GetAppID(void);

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
c_SteamUserStats_UploadLeaderboardScore(int64_t hLeaderboard, int32_t nScore);

void
c_SteamAPI_Shutdown(void);