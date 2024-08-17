/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
