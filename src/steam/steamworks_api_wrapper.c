#include <stdint.h>
#include <string.h>
#include "steamworks_api_wrapper.h"
#include "steamworks_c_wrapper.h"
#include "../util.h"
#include "../defines.h"
#include "../gui.h"
#include "../timer.h"

static const char *LB_HIGHSCORE	= "Highscore";
static const char *LB_KILLS	= "Most Kills";

static Achievement g_Achievements[] = {
	_ACH_ID(BAD_DOG, "Bad Dog"),
	_ACH_ID(THE_DOCTOR_IS_OUT, "The Doctor is Out"),
	_ACH_ID(LIGHTS_ON, "Omnidirectional light"),
	_ACH_ID(BACK_TO_WORK, "Back to work"),
	_ACH_ID(DRAGON_SLAYER, "Platinum dragon slayer")
};
static Uint8 numAchievements = 5;

static bool m_Initiated = false;
static bool m_bStatsReceived = false;
static Sint64 m_AppID = 0;
static Sint64 m_hHighscoreLeaderboard = 0;
static Sint64 m_hKillsLeaderboard = 0;

static Timer *requestDataTimer = NULL;

static bool
steam_request_stats(void)
{
	if (m_Initiated)
		return c_SteamUserStats_RequestCurrentStats();
	return false;
}

static void
stats_received(void)
{
	debug("Steam stats received");
	m_bStatsReceived = true;
}

static void
stats_stored(void)
{
	debug("Steam stats stored");
}

static void
leaderboard_received(Sint64 hLeaderboard, const char *name)
{
	debug("Leaderboard received: %s", name);
	if (strcmp(LB_HIGHSCORE, name) == 0)
		m_hHighscoreLeaderboard = hLeaderboard;
	else if (strcmp(LB_KILLS, name) == 0)
		m_hKillsLeaderboard = hLeaderboard;
}

void
steam_init()
{
	m_AppID = c_SteamAPI_Init();
	c_SteamAPI_SetCallbacks(stats_received, stats_stored, leaderboard_received);
	m_Initiated = m_AppID != 0;
	requestDataTimer = timer_create();
}

void steam_shutdown(void)
{
	c_SteamAPI_Shutdown();
	timer_destroy(requestDataTimer);
}

void
request_data_queue_run(void)
{
	if (!timer_started(requestDataTimer))
		timer_start(requestDataTimer);

	if (timer_get_ticks(requestDataTimer) > 1000) {
		if (!m_bStatsReceived)
			steam_request_stats();
		else if (!m_hHighscoreLeaderboard)
			c_SteamUserStats_FindLeaderboard(LB_HIGHSCORE);
		else if (!m_hKillsLeaderboard)
			c_SteamUserStats_FindLeaderboard(LB_KILLS);

		timer_start(requestDataTimer);
	}
}

void steam_run_callbacks(void)
{
	if (m_Initiated) {
		c_SteamAPI_RunCallbacks();
		request_data_queue_run();
	}
}

void steam_set_achievement(EAchievement eAch)
{
	for (Uint8 i = 0; i < numAchievements; ++i) {
		Achievement *a = &g_Achievements[i];
		if (a->m_eAchievementID == eAch && !a->m_bAchieved) {
			c_SteamUserStats_SetAchievement(g_Achievements[i].m_pchAchievementID);
			gui_log("You just earned the \"%s\" achievement", a->m_rgchName);
		}
	}
}

void steam_register_score(Sint32 nScore)
{
	if (!m_hHighscoreLeaderboard)
		return;
	c_SteamUserStats_UploadLeaderboardScore(m_hHighscoreLeaderboard, nScore);
}

void steam_register_kills(Sint32 nKills)
{
	if (!m_hKillsLeaderboard)
		return;
	c_SteamUserStats_UploadLeaderboardScore(m_hKillsLeaderboard, nKills);
}