#include <stdint.h>
#include <string.h>
#include "steamworks_api_wrapper.h"
#include "steamworks_c_wrapper.h"
#include "../util.h"
#include "../defines.h"
#include "../gui.h"
#include "../timer.h"
#include "../time.h"

static const char *LB_HIGHSCORE			= "Highscore";
static const char *LB_QUICKPLAY_HIGHSCORE	= "Quickplay Highscore";
static const char *LB_ARCADE_HIGHSCORE	= "Arcade Highscore";
static const char *LB_ROGUE_HIGHSCORE		= "Rogue Highscore";
static const char *LB_MAGE_HIGHSCORE		= "Mage Highscore";
static const char *LB_WARRIOR_HIGHSCORE		= "Warrior Highscore";
static const char *LB_KILLS			= "Most Kills";
static char *lb_weekly = NULL;

static Achievement g_Achievements[] = {
	_ACH_ID(BAD_DOG, "Bad Dog"),
	_ACH_ID(THE_DOCTOR_IS_OUT, "The Doctor is Out"),
	_ACH_ID(LIGHTS_ON, "Omnidirectional light"),
	_ACH_ID(BACK_TO_WORK, "Back to work"),
	_ACH_ID(DRAGON_SLAYER, "Platinum dragon slayer"),
	_ACH_ID(ROGUE_LIKE, "Rogue-like"),
	_ACH_ID(MAGICAL, "Magical")
};
static Uint8 numAchievements = 7;

static bool m_Initiated = false;
static Sint64 m_AppID = 0;
static Sint64 m_hHighscoreLeaderboard = 0;
static Sint64 m_hQpHighscoreLeaderboard = 0;
static Sint64 m_hArcadeHighscoreLeaderboard = 0;
static Sint64 m_hKillsLeaderboard = 0;
static Sint64 m_hRogueHighscore = 0;
static Sint64 m_hWarriorHighscore = 0;
static Sint64 m_hMageHighscore = 0;
static Sint64 m_hWeeklyHighscore = 0;

static Timer *requestDataTimer = NULL;

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
	else if (strcmp(LB_ROGUE_HIGHSCORE, name) == 0)
		m_hRogueHighscore = hLeaderboard;
	else if (strcmp(LB_MAGE_HIGHSCORE, name) == 0)
		m_hMageHighscore = hLeaderboard;
	else if (strcmp(LB_WARRIOR_HIGHSCORE, name) == 0)
		m_hWarriorHighscore = hLeaderboard;
	else if (strcmp(LB_QUICKPLAY_HIGHSCORE, name) == 0)
		m_hQpHighscoreLeaderboard = hLeaderboard;
	else if (strcmp(LB_ARCADE_HIGHSCORE, name) == 0)
		m_hArcadeHighscoreLeaderboard = hLeaderboard;
	else if (strcmp(lb_weekly, name) == 0)
		m_hWeeklyHighscore = hLeaderboard;
}

bool
steam_restart_needed()
{
	return c_SteamAPI_RestartAppIfNecessary();
}

void
steam_init()
{
	m_AppID = c_SteamAPI_Init();
	m_Initiated = m_AppID != 0;
	lb_weekly = time_get_weekly_lb_name();
	if (m_Initiated)
		c_SteamAPI_SetCallbacks(stats_stored, leaderboard_received);
	requestDataTimer = _timer_create();
}

void steam_shutdown(void)
{
	c_SteamAPI_Shutdown();
	timer_destroy(requestDataTimer);
	if (lb_weekly) 
		free(lb_weekly);
}

static void
request_data_queue_run(void)
{
	if (!timer_started(requestDataTimer))
		timer_start(requestDataTimer);

	if (timer_get_ticks(requestDataTimer) > 1000) {
		if (!m_hHighscoreLeaderboard)
			c_SteamUserStats_FindLeaderboard(LB_HIGHSCORE);
		else if (!m_hQpHighscoreLeaderboard)
			c_SteamUserStats_FindLeaderboard(LB_QUICKPLAY_HIGHSCORE);
		else if (!m_hArcadeHighscoreLeaderboard)
			c_SteamUserStats_FindLeaderboard(LB_ARCADE_HIGHSCORE);
		else if (!m_hKillsLeaderboard)
			c_SteamUserStats_FindLeaderboard(LB_KILLS);
		else if (!m_hRogueHighscore)
			c_SteamUserStats_FindLeaderboard(LB_ROGUE_HIGHSCORE);
		else if (!m_hMageHighscore)
			c_SteamUserStats_FindLeaderboard(LB_MAGE_HIGHSCORE);
		else if (!m_hWarriorHighscore)
			c_SteamUserStats_FindLeaderboard(LB_WARRIOR_HIGHSCORE);
		else if (!m_hWeeklyHighscore)
			c_SteamUserStats_FindOrCreateLeaderboard(lb_weekly);

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
	if (!m_Initiated)
		return;

	for (Uint8 i = 0; i < numAchievements; ++i) {
		Achievement *a = &g_Achievements[i];
		if (a->m_eAchievementID == eAch && !a->m_bAchieved) {
			c_SteamUserStats_SetAchievement(g_Achievements[i].m_pchAchievementID);
			gui_log("You just earned the \"%s\" achievement", a->m_rgchName);
		}
	}
}

void steam_register_score(Sint32 nScore, const int32_t *details, int32_t nDetails)
{
	if (!m_hHighscoreLeaderboard)
		return;
	c_SteamUserStats_UploadLeaderboardScore(m_hHighscoreLeaderboard, nScore, details, nDetails);
}

void steam_register_qp_score(Sint32 nScore, const int32_t *details, int32_t nDetails)
{
	if (!m_hQpHighscoreLeaderboard)
		return;
	c_SteamUserStats_UploadLeaderboardScore(m_hQpHighscoreLeaderboard, nScore, details, nDetails);
}

void steam_register_weekly_score(Sint32 nScore, const int32_t *details, int32_t nDetails)
{
	if (!m_hWeeklyHighscore)
		return;
	c_SteamUserStats_UploadLeaderboardScore(m_hWeeklyHighscore, nScore, details, nDetails);
}

void steam_register_arcade_score(Sint32 nScore, const int32_t * details, int32_t nDetails)
{
	if (!m_hArcadeHighscoreLeaderboard)
		return;
	c_SteamUserStats_UploadLeaderboardScore(m_hArcadeHighscoreLeaderboard, nScore, details, nDetails);
}

void steam_register_mage_score(Sint32 nScore, const int32_t * details, int32_t nDetails)
{
	if (!m_hMageHighscore)
		return;
	c_SteamUserStats_UploadLeaderboardScore(m_hMageHighscore, nScore, details, nDetails);
}

void steam_register_warrior_score(Sint32 nScore, const int32_t * details, int32_t nDetails)
{
	if (!m_hWarriorHighscore)
		return;
	c_SteamUserStats_UploadLeaderboardScore(m_hWarriorHighscore, nScore, details, nDetails);
}

void steam_register_rogue_score(Sint32 nScore, const int32_t * details, int32_t nDetails)
{
	if (!m_hRogueHighscore)
		return;
	c_SteamUserStats_UploadLeaderboardScore(m_hRogueHighscore, nScore, details, nDetails);
}

void steam_register_kills(Sint32 nKills, const int32_t *details, int32_t nDetails)
{
	if (!m_hKillsLeaderboard)
		return;
	c_SteamUserStats_UploadLeaderboardScore(m_hKillsLeaderboard, nKills, details, nDetails);
}
