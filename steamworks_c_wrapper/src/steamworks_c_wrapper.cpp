#include <steam_api.h>

extern "C" {
#include "steamworks_c_wrapper.h"
}

extern "C" void c_SteamAPI_Init()
{
	SteamAPI_Init();
}

extern "C" void c_SteamAPI_Shutdown()
{
	SteamAPI_Shutdown();
}

extern "C" bool c_SteamUserStats_RequestCurrentStats()
{
	return SteamUserStats()->RequestCurrentStats();
}

extern "C" bool c_SteamUserStats_SetAchievement(const char *pchName)
{
	return SteamUserStats()->SetAchievement(pchName);
}
