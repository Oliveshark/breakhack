#pragma once

#include <stdbool.h>
#include "../defines.h"

typedef enum EAchievement
{
	BAD_DOG = 0,
	THE_DOCTOR_IS_OUT = 1,
	LIGHTS_ON = 2,
	BACK_TO_WORK = 5,
	DRAGON_SLAYER = 6
} EAchievement;


#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }
typedef struct Achievement {
	EAchievement m_eAchievementID;
	const char *m_pchAchievementID;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
} Achievement;

void steam_init(void);

void steam_shutdown(void);

void steam_run_callbacks(void);

void steam_set_achievement(EAchievement eAch);

void steam_register_score(Sint32 nScore, const int32_t *details, int32_t nDetails);

void steam_register_kills(Sint32 nKills, const int32_t *details, int32_t nDetails);
