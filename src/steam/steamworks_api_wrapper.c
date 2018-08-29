#include <stdint.h>
#include "steamworks_api_wrapper.h"
#include "steamworks_c_wrapper.h"
#include "../util.h"
#include "../defines.h"
#include "../gui.h"

static Achievement g_Achievements[] = {
	_ACH_ID(BAD_DOG, "Bad Dog"),
	_ACH_ID(THE_DOCTOR_IS_OUT, "The Doctor is Out"),
	_ACH_ID(LIGHTS_ON, "Omnidirectional light"),
	_ACH_ID(BACK_TO_WORK, "Back to work"),
	_ACH_ID(DRAGON_SLAYER, "Platinum dragon slayer")
};
static Uint8 numAchievements = 5;

static bool m_Initiated = false;
static Sint64 m_AppID = 0;

static bool
steam_request_stats()
{
	if (m_Initiated)
		return c_SteamUserStats_RequestCurrentStats();
	return false;
}

static void
stats_received(void)
{
	debug("Steam stats received");
}

static void
stats_stored(void)
{
	debug("Steam stats stored");
}

void
steam_init()
{
	m_AppID = c_SteamAPI_Init();
	m_Initiated = m_AppID != 0;
	if (m_Initiated) {
		c_SteamAPI_SetCallbacks(stats_received, stats_stored);
		steam_request_stats();
	}
}

void steam_shutdown(void)
{
	c_SteamAPI_Shutdown();
}

void steam_run_callbacks(void)
{
	if (m_Initiated)
		c_SteamAPI_RunCallbacks();
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
