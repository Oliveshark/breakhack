#include <iostream>
#include "CSteamLeaderboard.h"

CSteamLeaderboard::CSteamLeaderboard() : m_hCurrentLeaderboard(0)
{
	// Nothing yet
}

void
CSteamLeaderboard::SetCurrent(SteamLeaderboard_t hCurrentLeaderboard)
{
	m_hCurrentLeaderboard = hCurrentLeaderboard;
}

void
CSteamLeaderboard::FindLeaderboard(const char *pchLeaderboardName )
{

	SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(pchLeaderboardName);
	m_callResultFindLeaderboard.Set(hSteamAPICall,
									this,
									&CSteamLeaderboard::OnFindLeaderboard);
}

bool
CSteamLeaderboard::UploadScore(int score, const int *details, int nDetails)
{
	if (!m_hCurrentLeaderboard)
		return false;

	SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(m_hCurrentLeaderboard,
																			k_ELeaderboardUploadScoreMethodKeepBest,
																			score,
																			details,
																			nDetails);
	m_callResultUploadScore.Set(hSteamAPICall, this, &CSteamLeaderboard::OnUploadScore);
	return true;
}

bool
CSteamLeaderboard::DownloadScores()
{
	if (!m_hCurrentLeaderboard) return false;
	SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries( m_hCurrentLeaderboard,
																				 k_ELeaderboardDataRequestGlobalAroundUser,
																				 -4,
																				 5);

	m_callResultDownloadScores.Set(hSteamAPICall, this, &CSteamLeaderboard::OnDownloadScores);
	return true;
}

void
CSteamLeaderboard::OnFindLeaderboard(LeaderboardFindResult_t *pCallback, bool bIOFailiure)
{
	if (!pCallback->m_bLeaderboardFound || bIOFailiure) {
		std::cerr << "Leaderboard could not be found" << std::endl;
		return;
	}
	m_hCurrentLeaderboard = pCallback->m_hSteamLeaderboard;
}

void
CSteamLeaderboard::OnUploadScore(LeaderboardScoreUploaded_t *pCallback, bool bIOFailiure)
{
	if (!pCallback->m_bSuccess || bIOFailiure)
		std::cerr << "Score could not be uploaded" << std::endl;
}

void
CSteamLeaderboard::OnDownloadScores(LeaderboardScoresDownloaded_t *pCallback, bool bIOFailiure)
{
	if (!bIOFailiure) {
		m_nLeaderboardEntries = std::min(pCallback->m_cEntryCount, 10);
		for (int index = 0; index < m_nLeaderboardEntries; index++) {
			SteamUserStats()->GetDownloadedLeaderboardEntry( pCallback->m_hSteamLeaderboardEntries,
															 index,
															 &m_leaderboardEntries[index],
															 NULL,
															 0);
		}
	}
}
