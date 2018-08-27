#pragma once

enum Achievements
{
	ACH_BAD_DOG = 0,
	ACH_THE_DOCTOR_IS_OUT = 1,
	ACH_LIGHTS_ON = 2,
	ACH_BACK_TO_WORK = 5,
};

struct Achievement
{
	Achievements m_eAchievementID;
	const char *m_pchAchievementID;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
};

void
c_SteamAPI_Init();

bool
c_steam_request_stats();

bool
c_steam_set_achievement(const char *id);

void
c_SteamAPI_Shutdown();
