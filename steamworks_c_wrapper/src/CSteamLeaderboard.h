#pragma once

#include <steam_api.h>

class CSteamLeaderboard
{
	private:
		SteamLeaderboard_t m_hCurrentLeaderboard;
		LeaderboardEntry_t m_leaderboardEntries[10];
		int m_nLeaderboardEntries = 0;

	public:
		CSteamLeaderboard();

		void SetCurrent(SteamLeaderboard_t hCurrentLeaderboard);

		void FindLeaderboard(const char *pchLeaderboardName );
		bool UploadScore(int score, const int *details, int nDetails);
		bool DownloadScores();

		void OnFindLeaderboard(LeaderboardFindResult_t *pCallback, bool bIOFailiure);
		CCallResult<CSteamLeaderboard, LeaderboardFindResult_t> m_callResultFindLeaderboard;

		void OnUploadScore(LeaderboardScoreUploaded_t *pCallback, bool bIOFailiure);
		CCallResult<CSteamLeaderboard, LeaderboardScoreUploaded_t> m_callResultUploadScore;

		void OnDownloadScores(LeaderboardScoresDownloaded_t *pCallback, bool bIOFailiure);
		CCallResult<CSteamLeaderboard, LeaderboardScoresDownloaded_t> m_callResultDownloadScores;
};
